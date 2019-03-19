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

#include "DaycareGenerator.h"

#include "geopop/Daycare.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "util/RnMan.h"

#include <trng/discrete_dist.hpp>
#include <geopop/GeoGridConfig.h>

namespace geopop {

using namespace std;

void DaycareGenerator::Apply(shared_ptr<GeoGrid> geoGrid, const GeoGridConfig& geoGridConfig,
                             unsigned int& contactCenterCounter)
{
        // 1. given the number of persons of daycare age, calculate number of daycares; daycares
        //    have 3 pupils on average
        // 2. assign daycares to a location by using a discrete distribution which reflects the
        //    relative number of pupils for that location; the relative number of pupils is set
        //    to the relative population w.r.t the total population.

        const auto pupilCount = geoGridConfig.popInfo.popcount_daycare;
        const auto schoolCount =
            static_cast<unsigned int>(ceil(pupilCount / static_cast<double>(geoGridConfig.pools.daycare_size)));

        vector<double> weights;
        for (const auto& loc : *geoGrid) {
                weights.push_back(loc->GetRelativePopulationSize());
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rnManager[0].variate_generator(trng::discrete_dist(weights.begin(), weights.end()));

        for (auto i = 0U; i < schoolCount; i++) {
                const auto loc = (*geoGrid)[dist()];
                const auto day = make_shared<Daycare>(contactCenterCounter++);
                day->Fill(geoGridConfig, geoGrid);
                loc->AddContactCenter(day);
        }
}

} // namespace geopop
