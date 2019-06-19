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

template <>
void Populator<stride::ContactType::Id::PreSchool>::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->trace("Starting to populate PreSchools");

        // for every location
        for (const auto& loc : *geoGrid.m_locationGrid) {
                if (loc->getData<Location>()->GetPopCount() == 0) {
                        continue;
                }

                // 1. find all preschools in an area of 10-k*10 km
                const auto& nearByPreSchoolsPools = geoGrid.GetNearbyPools(Id::PreSchool, *loc);

                AssertThrow(!nearByPreSchoolsPools.empty(), "No Daycares found!", m_logger);

                const auto dist =
                    m_rn_man.GetUniformIntGenerator(0, static_cast<int>(nearByPreSchoolsPools.size()), 0U);

                // 2. for every todler assign a class
                for (const auto& hhPool : loc->getData<Location>()->RefPools(Id::Household)) {
                        for (Person* p : *hhPool) {
                                if (AgeBrackets::PreSchool::HasAge(p->GetAge()) &&
                                    m_rn_man.MakeWeightedCoinFlip(
                                        geoGridConfig.params.at(loc->getData<Location>()->GetProvince())
                                            .participation_preschool)) {
                                        // this person is a todler
                                        auto& c = nearByPreSchoolsPools[dist()];
                                        c->AddMember(p);
                                        p->SetPoolId(Id::PreSchool, c->GetId());
                                }
                        }
                }
        }

        m_logger->trace("Done populating PreSchools");
}

} // namespace geopop
