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

#include "WorkplacePopulator.h"

#include "gengeopop/GeoGrid.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/Household.h"
#include "gengeopop/Location.h"
#include "gengeopop/Workplace.h"
#include "pool/AgeBrackets.h"
#include "pool/ContactPool.h"
#include "util/ExcAssert.h"

#include <trng/uniform_int_dist.hpp>
#include <utility>

namespace gengeopop {

using namespace std;
using namespace stride;
using namespace stride::ContactPoolType;
using namespace util;

WorkplacePopulator::WorkplacePopulator(RnMan& rn_manager, shared_ptr<spdlog::logger> logger)
    : Populator(rn_manager, move(logger)), m_currentLoc(nullptr), m_geoGrid(nullptr), m_geoGridConfig(),
      m_workplacesInCity(), m_fractionCommutingStudents(0), m_nearByWorkplaces(), m_distNonCommuting(),
      m_commutingLocations(), m_disCommuting()
{
}

void WorkplacePopulator::Apply(shared_ptr<GeoGrid> geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->info("Starting to populate Workplaces");

        m_geoGrid                   = geoGrid;
        m_geoGridConfig             = geoGridConfig;
        m_fractionCommutingStudents = 0;
        m_workplacesInCity.clear();
        m_currentLoc           = nullptr;
        m_assignedTo0          = 0;
        m_assignedCommuting    = 0;
        m_assignedNotCommuting = 0;
        m_distNonCommuting     = discreteDist();
        m_nearByWorkplaces.clear();
        m_disCommuting = discreteDist();
        m_commutingLocations.clear();

        CalculateFractionCommutingStudents();
        CalculateWorkplacesInCity();

        // for every location
        for (const auto& loc : *geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                m_currentLoc = loc;
                CalculateCommutingLocations();
                CalculateNearbyWorkspaces();

                // 2. for everyone of working age: decide between work or college (iff of College age)
                for (const auto& household : loc->GetContactCentersOfType<Household>()) {
                        auto contactPool = household->GetPools()[0];
                        for (auto p : *contactPool) {
                                if (AgeBrackets::Workplace::HasAge((p->GetAge()))) {
                                        bool isStudent      = MakeChoice(geoGridConfig.input.fraction_1826_student);
                                        bool isActiveWorker = MakeChoice(geoGridConfig.input.fraction_1865_active);

                                        if ((AgeBrackets::College::HasAge(p->GetAge()) && !isStudent) ||
                                            isActiveWorker) {
                                                AssignActive(p);
                                        } else {
                                                // this person has no employment
                                                p->SetPoolId(Id::Work, 0);
                                                m_assignedTo0++;
                                        }
                                }
                        }
                }
        }

        m_logger->info("Populated workplaces, assigned to 0 {}, assigned (commuting) {} assigned (not commuting) {} ",
                       m_assignedTo0, m_assignedCommuting, m_assignedNotCommuting);
}

void WorkplacePopulator::CalculateFractionCommutingStudents()
{
        m_fractionCommutingStudents = 0;
        if (static_cast<bool>(m_geoGridConfig.input.fraction_active_commuters) &&
            m_geoGridConfig.popInfo.popcount_1865_active) {
                m_fractionCommutingStudents =
                    (m_geoGridConfig.popInfo.popcount_1826_student * m_geoGridConfig.input.fraction_student_commuters) /
                    (m_geoGridConfig.popInfo.popcount_1865_active * m_geoGridConfig.input.fraction_active_commuters);
        }
}

void WorkplacePopulator::CalculateWorkplacesInCity()
{
        for (const shared_ptr<Location>& loc : *m_geoGrid) {
                vector<ContactPool*> contactPools;
                for (const auto& wp : loc->GetContactCentersOfType<Workplace>()) {
                        contactPools.insert(contactPools.end(), wp->begin(), wp->end());
                }

                auto disPools = m_rnManager[0].variate_generator(
                    trng::uniform_int_dist(0, static_cast<trng::uniform_int_dist::result_type>(contactPools.size())));

                m_workplacesInCity[loc.get()] = {contactPools, disPools};
        }
}

void WorkplacePopulator::AssignActive(Person* person)
{
        // this person is (student and active) or active
        if (!m_commutingLocations.empty() && MakeChoice(m_geoGridConfig.input.fraction_active_commuters)) {
                // this person is commuting
                const auto& info = m_workplacesInCity[m_commutingLocations[m_disCommuting()]];
                const auto  id   = info.second(); // id of the location this person is commuting to
                info.first[id]->AddMember(person);
                person->SetPoolId(Id::Work, info.first[id]->GetId());
                m_assignedCommuting++;
        } else {
                // this person is not commuting
                const auto id = m_distNonCommuting();
                m_nearByWorkplaces[id]->AddMember(person);
                person->SetPoolId(Id::Work, m_nearByWorkplaces[id]->GetId());
                m_assignedNotCommuting++;
        }
}

void WorkplacePopulator::CalculateCommutingLocations()
{
        // find all Workplaces were employees from this location commute to
        m_commutingLocations.clear();
        m_disCommuting = discreteDist();

        vector<double> commutingWeights;
        for (const pair<Location*, double>& commute : m_currentLoc->GetOutgoingCommutingCities()) {
                const auto& workplaces = commute.first->GetContactCentersOfType<Workplace>();
                if (!workplaces.empty()) {
                        m_commutingLocations.push_back(commute.first);
                        const auto weight = commute.second - (commute.second * m_fractionCommutingStudents);
                        commutingWeights.push_back(weight);
                        ExcAssert(weight >= 0 && weight <= 1 && !isnan(weight),
                                  "Invalid weight due to data in WorkplacePopulator, weight: " + to_string(weight));
                }
        }

        if (!commutingWeights.empty()) {
                m_disCommuting = m_rnManager[0].variate_generator(
                    trng::discrete_dist(commutingWeights.begin(), commutingWeights.end()));
        }
}

void WorkplacePopulator::CalculateNearbyWorkspaces()
{
        m_nearByWorkplaces = GetPoolInIncreasingRadius<Workplace>(m_geoGrid, m_currentLoc);
        m_distNonCommuting = m_rnManager[0].variate_generator(
            trng::uniform_int_dist(0, static_cast<trng::uniform_int_dist::result_type>(m_nearByWorkplaces.size())));
}

} // namespace gengeopop
