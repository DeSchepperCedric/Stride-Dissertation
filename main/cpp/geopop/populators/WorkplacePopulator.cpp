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

double GetWorkplaceWeight(const GeoGridConfig& geoGridConfig, const stride::ContactPool* pool, const unsigned int index)
{
        // pool is full but possibility to transform to larger workplace
        if (pool->size() >= geoGridConfig.refWP.max[index] && index != (geoGridConfig.refWP.max.size()) - 1) {
                return 0.000001;
        }
        // pool is full and largest workplace possible (only when absolutely necessary)
        else if (pool->size() >= geoGridConfig.refWP.max[index]) {

                return 0.00000000001 / (pool->size() - geoGridConfig.refWP.max[index] + 1);
        }
        // pool size is below minimum value
        else if (pool->size() < geoGridConfig.refWP.min[index]) {
                return (1 - geoGridConfig.refWP.ratios[index]);
        } else {
                return (1 - geoGridConfig.refWP.ratios[index]) / 10;
        }
}

unsigned int GetWorkplaceIndex(std::unordered_map<unsigned int, unsigned int>& poolTypes, const unsigned int id,
                               std::function<int()>& gen)
{
        auto it = poolTypes.find(id);
        if (it == poolTypes.end()) {
                auto index    = gen();
                poolTypes[id] = index;
                return index;

        } else {
                return it->second;
        }
}

template <>
void Populator<stride::ContactType::Id::Workplace>::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->trace("Starting to populate Workplaces");
        auto genCommute{function<int()>()};
        auto genNonCommute{function<int()>()};
        auto gen{function<int()>()};
        auto genWorkPlaceSize{function<int()>()};

        vector<ContactPool*> nearbyWp{};

        vector<Location*> commuteLocations{};

        // key : pool ID
        // value : index of pool type
        std::unordered_map<unsigned int, unsigned int> poolTypes;

        const bool wp_types_present = !geoGridConfig.refWP.ratios.empty();

        // discrete generator to decide workplace type
        if (wp_types_present) {
                genWorkPlaceSize = m_rn_man.GetDiscreteGenerator(geoGridConfig.refWP.ratios, 0U);
        }
        // --------------------------------------------------------------------------------
        // For every location, if populated ...
        // --------------------------------------------------------------------------------
        for (const auto& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                const auto prov              = loc->GetProvince();
                const auto participWorkplace = geoGridConfig.params.at(prov).participation_workplace;
                const auto popCollege = static_cast<unsigned int>(geoGridConfig.regionsInfo.at(prov).fraction_college *
                                                                  geoGridConfig.params.at(prov).pop_size);
                const auto popWorkplace = static_cast<unsigned int>(
                    geoGridConfig.regionsInfo.at(prov).fraction_workplace * geoGridConfig.params.at(prov).pop_size);
                const auto fracCollegeCommute   = geoGridConfig.params.at(prov).fraction_college_commuters;
                const auto fracWorkplaceCommute = geoGridConfig.params.at(prov).fraction_workplace_commuters;

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
                // When dealing with different workplace types, the generator will be updated later
                // --------------------------------------------------------------------------------
                nearbyWp      = geoGrid.GetNearbyPools(Id::Workplace, *loc);
                genNonCommute = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(nearbyWp.size()), 0U);

                // --------------------------------------------------------------------------------
                // For everyone of working age: decide between work or college (iff of College age)
                // --------------------------------------------------------------------------------
                for (auto& hhPool : loc->RefPools(Id::Household)) {

                        for (auto person : *hhPool) {

                                // NOTICE: logic below requires that CollegePopulator has already executed
                                // such that we can identify the college students.
                                // If this person is not in the age bracket for college/work/unemployed
                                // or if the perosn is in the age bracket but is a student we are done here.
                                if (!Workplace::HasAge(person->GetAge()) || (person->GetPoolId(Id::College) != 0)) {
                                        continue;
                                }

                                // We are dealing with a non-student person of the age bracket for work,
                                // flip coin to decide whether they are actually employed.
                                bool isActiveWorker = m_rn_man.MakeWeightedCoinFlip(participWorkplace);
                                if (isActiveWorker) {
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
                                                if (wp_types_present) {
                                                        // -------------------------------------
                                                        // Handling workplace size distribution
                                                        // -------------------------------------
                                                        std::vector<double> weightsCommutePools;
                                                        for (int i = 0; i < s; i++) {
                                                                const unsigned int index = geopop::GetWorkplaceIndex(
                                                                    poolTypes, pools[i]->GetId(), genWorkPlaceSize);
                                                                const auto weight =
                                                                    GetWorkplaceWeight(geoGridConfig, pools[i], index);
                                                                weightsCommutePools.push_back(weight);
                                                                AssertThrow(
                                                                    weight >= 0.0 && weight <= 1.0 && !isnan(weight),
                                                                    "Invalid weight: " + to_string(weight), m_logger);
                                                        }
                                                        gen = m_rn_man.GetDiscreteGenerator(weightsCommutePools, 0U);
                                                } else {
                                                        gen = m_rn_man.GetUniformIntGenerator(0, s);
                                                }
                                                auto pool      = pools[gen()];
                                                auto pool_type = poolTypes[pool->GetId()];

                                                pool->AddMember(person);
                                                person->SetPoolId(Id::Workplace, pool->GetId());
                                                // increase workplace type to larger size
                                                if (wp_types_present &&
                                                    pool->size() > geoGridConfig.refWP.max[pool_type] &&
                                                    pool_type < geoGridConfig.refWP.ratios.size() - 1) {
                                                        poolTypes[pool->GetId()] += 1;
                                                }

                                        } else {
                                                // ----------------------------
                                                // this person does not commute
                                                // ----------------------------
                                                if (wp_types_present) {
                                                        // -------------------------------------
                                                        // Handling workplace size distribution
                                                        // -------------------------------------
                                                        std::vector<double> weightsNonCommutePools;
                                                        std::vector<int>    tempIndex;
                                                        for (auto& wp : nearbyWp) {
                                                                const auto index = geopop::GetWorkplaceIndex(
                                                                    poolTypes, wp->GetId(), genWorkPlaceSize);
                                                                const auto weight = geopop::GetWorkplaceWeight(
                                                                    geoGridConfig, wp, index);
                                                                weightsNonCommutePools.push_back(weight);
                                                                AssertThrow(
                                                                    weight >= 0.0 && weight <= 1.0 && !isnan(weight),
                                                                    "Invalid weight: " + to_string(weight), m_logger);
                                                        }
                                                        genNonCommute =
                                                            m_rn_man.GetDiscreteGenerator(weightsNonCommutePools, 0U);
                                                }

                                                auto pool = nearbyWp[genNonCommute()];

                                                const auto pool_type = poolTypes[pool->GetId()];

                                                pool->AddMember(person);
                                                person->SetPoolId(Id::Workplace, pool->GetId());

                                                // increase workplace type to larger size
                                                if (wp_types_present &&
                                                    pool->size() > geoGridConfig.refWP.max[pool_type] &&
                                                    pool_type < geoGridConfig.refWP.ratios.size() - 1) {

                                                        poolTypes[pool->GetId()] += 1;
                                                }
                                        }
                                }
                        }
                }
        }

        m_logger->trace("Done populating Workplaces");
}

} // namespace geopop
