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

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template <>
void Generator<stride::ContactType::Id::K12School>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig)
{
        // 1. given the number of persons of school age, calculate number of schools
        // 2. assign schools to a location by using a discrete distribution which reflects the
        //    relative number of pupils for that location; the relative number of pupils is set
        //    to the relative population w.r.t the total population.

        for (const auto& it : ggConfig.regionsInfo) {
                // Generate schools per region, this way regions with a younger population have more schools
                auto           popCount      = 0U;
                auto           majorPopCount = 0U;
                vector<double> weights;
                for (const auto& loc : geoGrid) {
                        if (loc->GetProvince() == it.first) {
                                weights.push_back(loc->GetPopFraction());
                                if (loc->IsMajor()) {
                                        majorPopCount += loc->GetPopCount();
                                } else {
                                        popCount += loc->GetPopCount();
                                }
                        } else {
                                // To make sure the index in weights corresponds to the correct location in the geogrid
                                weights.push_back(0.0);
                        }
                }
                const auto pupilCount = static_cast<unsigned int>(
                    ceil(it.second.fraction_k12school * popCount + it.second.major_fraction_k12school * majorPopCount));
                const auto schoolCount =
                    static_cast<unsigned int>(ceil(pupilCount / static_cast<double>(ggConfig.people[Id::K12School])));

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
}

} // namespace geopop
