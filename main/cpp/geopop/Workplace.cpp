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

#include "Workplace.h"

#include "GeoGrid.h"
#include "GeoGridConfig.h"
#include "pop/Population.h"

namespace geopop {

void Workplace::SetupPools(const GeoGridConfig& /* geoGridConfig */, stride::Population* pop)
{
        auto& poolSys = pop->RefPoolSys();

        // TODO CheckThisAlgorithm
        // for (std::size_t i = 0; i < geoGridConfig.pools.pools_per_workplace; ++i) {
        const auto p = poolSys.CreateContactPool(stride::ContactType::Id::Workplace);
        RegisterPool(p);
        //}
}

} // namespace geopop
