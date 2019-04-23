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
#include "pop/Person.h"
#include "util/Assert.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template<>
void Populator<stride::ContactType::Id::Daycare>::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->info("Starting to populate Daycares");

        // for every location
        for (const auto& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                // 1. find all daycares in an area of 10-k*10 km
                const auto& nearByDaycaresPools = geoGrid.GetNearbyPools(Id::Daycare, *loc);

                AssertThrow(!nearByDaycaresPools.empty(), "No Daycares found!", m_logger);

                const auto dist =
                    m_rn_man.GetUniformIntGenerator(0, static_cast<int>(nearByDaycaresPools.size()), 0U);

                // 2. for every baby assign a class
                for (const auto& hhPool : loc->RefPools(Id::Household)) {
                        for (Person* p : *hhPool) {
                                if (AgeBrackets::Daycare::HasAge(p->GetAge()) &&
                                    m_rn_man.MakeWeightedCoinFlip(geoGridConfig.param.participation_daycare)) {
                                        // this person is a baby
                                        auto& c = nearByDaycaresPools[dist()];
                                        c->AddMember(p);
                                        p->SetPoolId(Id::Daycare, c->GetId());
                                }
                        }
                }
        }

        m_logger->trace("Done populating Daycares");
}

} // namespace geopop
