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

#include "Generator.h"

#include "util/Assert.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template <>
void Generator<stride::ContactType::Id::Daycare>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig)
{
        // 1. given the number of persons of daycare age, calculate number of daycares; daycares
        //    have 3 pupils on average
        // 2. assign daycares to a location by using a discrete distribution which reflects the
        //    relative number of pupils for that location; the relative number of pupils is set
        //    to the relative population w.r.t the total population.

        auto pupilCount = 0U;
        for (const auto & it : ggConfig.regionsInfo){
                pupilCount += it.second.popcount_daycare;
//                pupilCount += it.second.major_popcount_daycare;
        }
        const auto schoolCount =
            static_cast<unsigned int>(ceil(pupilCount / static_cast<double>(ggConfig.people[Id::Daycare])));

        vector<double> weights;
        for (const auto& loc : geoGrid) {
                weights.push_back(loc->GetPopFraction());
        }

        if (weights.empty()) {
                // trng can't handle empty vectors
                return;
        }

        const auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        auto       pop  = geoGrid.GetPopulation();

        for (auto i = 0U; i < schoolCount; i++) {
                const auto loc = geoGrid[dist()];
                AddPools(*loc, pop, ggConfig);
        }
}

} // namespace geopop
