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
void Generator<stride::ContactType::Id::Household>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig)
{
        for (const auto& it : ggConfig.regionsInfo) {
                if (it.first == -1) {
                        continue;
                }
                vector<double> weights;
                vector<double> majorWeights;
                auto           majorPop = 0u;
                for (const auto& loc : geoGrid) {
                        if (loc->GetProvince() == (unsigned)it.first) {
                                if (loc->IsMajor()) {
                                        majorWeights.push_back(loc->GetPopFraction());
                                        weights.push_back(0.0);
                                        majorPop += loc->GetPopCount();
                                } else {
                                        majorWeights.push_back(0.0);
                                        weights.push_back(loc->GetPopFraction());
                                }
                        } else {
                                // To make sure the index in weights corresponds to the correct location in the geogrid
                                weights.push_back(0.0);
                                majorWeights.push_back(0.0);
                        }
                }

                // if this holds true, majorWeights will also be empty
                if (weights.empty()) {
                        // trng can't handle empty vectors
                        return;
                }

                const auto dist      = m_rn_man.GetDiscreteGenerator(weights, 0U);
                const auto majorDist = m_rn_man.GetDiscreteGenerator(majorWeights, 0U);
                auto       pop       = geoGrid.GetPopulation();
                const auto major_count_households =
                    static_cast<unsigned int>(round((double)it.second.major_count_households *
                                                    ((double)majorPop / ggConfig.params.at(it.first).pop_size)));
                const auto count_households = static_cast<unsigned int>(
                    round((double)it.second.count_households *
                          (1.0 - ((double)majorPop / ggConfig.params.at(it.first).pop_size))));

                for (auto i = 0U; i < count_households; i++) {
                        const auto loc = geoGrid[dist()];
                        AddPools(*loc, pop, ggConfig);
                }

                for (auto i = 0U; i < major_count_households; i++) {
                        const auto loc = geoGrid[majorDist()];
                        AddPools(*loc, pop, ggConfig);
                }
        }
}

} // namespace geopop
