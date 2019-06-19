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

#include "GeoGrid.h"

#include "contact/ContactPool.h"
#include "geopop/Location.h"
#include "geopop/geo/GeoAggregator.h"
#include "geopop/geo/GeoGridKdTree.h"
#include "pop/Population.h"

#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>

namespace geopop {

using namespace std;
using stride::ContactPool;
using stride::ContactType::Id;

GeoGrid::GeoGrid(stride::Population* population)
    : m_locationGrid(std::make_shared<geopop::LocationGrid<Location>>()), m_locations(), m_population(population),
      m_id_to_index()
{
}

vector<ContactPool*> GeoGrid::GetNearbyPools(Id id, const EnhancedCoordinate& start, double startRadius) const
{
        double               currentRadius = startRadius;
        vector<ContactPool*> pools;

        while (pools.empty()) {
                for (const EnhancedCoordinate* nearLoc : m_locationGrid->LocationsInRadius(start, currentRadius)) {
                        const auto& locPool = nearLoc->EnhancedCoordinate::getData<Location>()->CRefPools(id);
                        pools.insert(pools.end(), locPool.begin(), locPool.end());
                }
                currentRadius *= 2;
                if (currentRadius == numeric_limits<double>::infinity()) {
                        break;
                }
        }
        return pools;
}

void GeoGrid::addLocation(std::shared_ptr<geopop::Location> loc, std::shared_ptr<geopop::EnhancedCoordinate> coor)
{
        m_locationGrid->AddData(move(coor));
        m_locations.push_back(loc);
        m_id_to_index[loc->GetID()] = static_cast<unsigned int>(m_locations.size() - 1);
}

} // namespace geopop
