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
#include <geopop/GeoGridConfig.h>

#include "util/Assert.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

template <>
void Generator<stride::ContactType::Id::Workplace>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig)
{
        // 1. active people count and the commuting people count are given
        // 2. count the workplaces
        // 3. count the working people at each location = #residents + #incoming commuters - #outgoing commuters
        // 4. use that information for the distribution
        // 5. assign each workplaces to a location

        for (const auto& it : ggConfig.regionsInfo) {
                // = for each location #residents + #incoming commuting people - #outgoing commuting people
                vector<double> weights;
                vector<double> majorWeights;
                auto           popCount      = 0U;
                auto           majorPopCount = 0U;
                for (const auto& loc : geoGrid) {
                        if (loc->GetProvince() == it.first) {
                                const double ActivePeopleCount =
                                    (loc->GetPopCount() +
                                     loc->GetIncomingCommuteCount(
                                         ggConfig.params.at(loc->GetProvince()).fraction_workplace_commuters) -
                                     loc->GetOutgoingCommuteCount(
                                         ggConfig.params.at(loc->GetProvince()).fraction_workplace_commuters) *
                                         ggConfig.params.at(loc->GetProvince()).participation_workplace);

                                const double weight = ActivePeopleCount; // / EmployeeCount;
                                AssertThrow(weight >= 0 && weight <= 1 && !std::isnan(weight),
                                            "Invalid weight: " + to_string(weight), m_logger);
                                if (loc->IsMajor()) {
                                        majorPopCount += loc->GetPopCount();
                                        weights.push_back(0.0);
                                        majorWeights.push_back((weight));

                                } else {
                                        popCount += loc->GetPopCount();
                                        weights.push_back(weight);
                                        majorWeights.push_back((0.0));
                                }
                        } else {
                                weights.push_back(0.0);
                                majorWeights.push_back((0.0));
                        }
                }

                const auto EmployeeCount = static_cast<unsigned int>(it.second.fraction_workplace * popCount);
                const auto WorkplaceSize = ggConfig.refWP.average_workplace_size == 0U
                                               ? ggConfig.people[Id::Workplace]
                                               : ggConfig.refWP.average_workplace_size;
                const auto WorkplacesCount =
                    static_cast<unsigned int>(ceil(EmployeeCount / static_cast<double>(WorkplaceSize)));

                const auto majorEmployeeCount =
                    static_cast<unsigned int>(it.second.major_fraction_workplace * majorPopCount);

                const auto majorWorkplacesCount =
                    static_cast<unsigned int>(ceil(majorEmployeeCount / static_cast<double>(WorkplaceSize)));

                if (weights.empty()) {
                        // trng can't handle empty vectors
                        return;
                }

                for (auto& w : weights) {
                        w /= EmployeeCount;
                }
                for (auto& w : majorWeights) {
                        w /= majorEmployeeCount;
                }

                const auto dist      = m_rn_man.GetDiscreteGenerator(weights, 0U);
                const auto majorDist = m_rn_man.GetDiscreteGenerator(majorWeights, 0U);
                auto       pop       = geoGrid.GetPopulation();

                for (auto i = 0U; i < WorkplacesCount; i++) {
                        const auto loc = geoGrid[dist()];
                        AddPools(*loc, pop, ggConfig);
                }
                for (auto i = 0U; i < majorWorkplacesCount; i++) {
                        const auto loc = geoGrid[majorDist()];
                        AddPools(*loc, pop, ggConfig);
                }
        }
}

} // namespace geopop
