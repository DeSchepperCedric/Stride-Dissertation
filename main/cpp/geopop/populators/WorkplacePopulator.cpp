/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "Populator.h"

#include "contact/AgeBrackets.h"
#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "util/Assert.h"

#include <geopop/GeoGridConfig.h>
#include <utility>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::AgeBrackets;
using namespace util;

template <>
void Populator<stride::ContactType::Id::Workplace>::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->trace("Starting to populate Workplaces");

        auto genCommute{function<int()>()};
        auto genNonCommute{function<int()>()};
        auto gen{function<int()>()};

        vector<ContactPool*> nearbyWp{};
        vector<Location*>    commuteLocations{};


        // --------------------------------------------------------------------------------
        // For every location, if populated ...
        // --------------------------------------------------------------------------------
        for (const auto& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                const auto participCollege      = geoGridConfig.params.at(loc->GetProvince()).participation_college;
                const auto participWorkplace    = geoGridConfig.params.at(loc->GetProvince()).participation_workplace;
                const auto popCollege           = geoGridConfig.regionsInfo.at(loc->GetProvince()).popcount_college;
                const auto popWorkplace         = geoGridConfig.regionsInfo.at(loc->GetProvince()).popcount_workplace;
                const auto fracCollegeCommute   = geoGridConfig.params.at(loc->GetProvince()).fraction_college_commuters;
                const auto fracWorkplaceCommute = geoGridConfig.params.at(loc->GetProvince()).fraction_workplace_commuters;

                double fracCommuteStudents = 0.0;
                if (static_cast<bool>(fracWorkplaceCommute) && popWorkplace) {
                        fracCommuteStudents = (popCollege * fracCollegeCommute) / (popWorkplace * fracWorkplaceCommute);
                }

                // --------------------------------------------------------------------------------
                // Find all Workplaces where employees from this location commute to
                // --------------------------------------------------------------------------------
                commuteLocations.clear();
                genCommute = function<int()>();

                vector<double> commutingWeights;
                for (const pair<Location*, double>& commute : loc->CRefOutgoingCommutes()) {
                        const auto& workplaces = commute.first->RefPools(Id::Workplace);
                        if (!workplaces.empty()) {
                                commuteLocations.push_back(commute.first);
                                const auto weight = commute.second - (commute.second * fracCommuteStudents);
                                commutingWeights.push_back(weight);
                                AssertThrow(weight >= 0.0 && weight <= 1.0 && !isnan(weight),
                                            "Invalid weight: " + to_string(weight), m_logger);
                        }
                }

                if (!commutingWeights.empty()) {
                        genCommute = m_rn_man.GetDiscreteGenerator(commutingWeights, 0U);
                }

                // --------------------------------------------------------------------------------
                // Set NearbyWorkspacePools and associated generator
                // --------------------------------------------------------------------------------
                nearbyWp = geoGrid.GetNearbyPools(Id::Workplace, *loc);

                if (!geoGridConfig.refWP.ratios.empty()) {
                        auto genWorkPlaceSize = m_rn_man.GetDiscreteGenerator(geoGridConfig.refWP.ratios, 0U);
                        std::vector<double> weightsNonCommutePools;
                        for (int i = 0; i < static_cast<int>(nearbyWp.size()); i++) {
                                auto weight = 1 - geoGridConfig.refWP.ratios[genWorkPlaceSize()];

                                weightsNonCommutePools.push_back(weight);
                                AssertThrow(weight >= 0.0 && weight <= 1.0 && !isnan(weight),
                                            "Invalid weight: " + to_string(weight), m_logger);
                        }
                        genNonCommute = m_rn_man.GetDiscreteGenerator(weightsNonCommutePools, 0U);
                } else {
                        genNonCommute = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(nearbyWp.size()), 0U);
                }

                // --------------------------------------------------------------------------------
                // For everyone of working age: decide between work or college (iff of College age)
                // --------------------------------------------------------------------------------
                for (auto& hhPool : loc->RefPools(Id::Household)) {
                        for (auto person : *hhPool) {
                                if (!Workplace::HasAge(person->GetAge())) {
                                        continue;
                                }

                                bool isStudent      = m_rn_man.MakeWeightedCoinFlip(participCollege);
                                bool isActiveWorker = m_rn_man.MakeWeightedCoinFlip(participWorkplace);

                                if ((College::HasAge(person->GetAge()) && !isStudent) || isActiveWorker) {
                                        // ---------------------------------------------
                                        // this person is employed
                                        // ---------------------------------------------
                                        const auto isCommuter = m_rn_man.MakeWeightedCoinFlip(fracWorkplaceCommute);
                                        if (!commuteLocations.empty() && isCommuter) {
                                                // --------------------------------------------------------------
                                                // this person commutes to the Location and in particular to Pool
                                                // --------------------------------------------------------------
                                                auto& pools = commuteLocations[genCommute()]->RefPools(Id::Workplace);
                                                auto  s     = static_cast<int>(pools.size());

                                                if (!geoGridConfig.refWP.ratios.empty()) {
                                                        auto genWorkPlaceSize = m_rn_man.GetDiscreteGenerator(
                                                            geoGridConfig.refWP.ratios, 0U);
                                                        std::vector<double> weightsCommutePools;
                                                        for (int i = 0; i < s; i++) {
                                                                auto weight =
                                                                    1 - geoGridConfig.refWP.ratios[genWorkPlaceSize()];

                                                                weightsCommutePools.push_back(weight);

                                                                AssertThrow(
                                                                    weight >= 0.0 && weight <= 1.0 && !isnan(weight),
                                                                    "Invalid weight: " + to_string(weight), m_logger);
                                                        }
                                                        gen = m_rn_man.GetDiscreteGenerator(weightsCommutePools, 0U);
                                                } else {
                                                        gen = m_rn_man.GetUniformIntGenerator(0, s);
                                                }

                                                auto pool = pools[gen()];
                                                // so that's it
                                                pool->AddMember(person);
                                                person->SetPoolId(Id::Workplace, pool->GetId());
                                        } else {
                                                // ----------------------------
                                                // this person does not commute
                                                // ----------------------------

                                                const auto idraw = genNonCommute();
                                                nearbyWp[idraw]->AddMember(person);
                                                person->SetPoolId(Id::Workplace, nearbyWp[idraw]->GetId());
                                        }
                                } else {
                                        // -----------------------------
                                        // this person has no employment
                                        // -----------------------------
                                        person->SetPoolId(Id::Workplace, 0);
                                }
                        }
                }
        }

        m_logger->trace("Done populating Workplaces");
}

} // namespace geopop
