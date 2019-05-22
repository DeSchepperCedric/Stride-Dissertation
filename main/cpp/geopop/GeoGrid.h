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

#pragma once

#include "contact/ContactType.h"
#include "geopop/geo/GeoGridKdTree.h"

#include "geopop/Location.h"

#include "LocationGrid.h"

#include <set>
#include <unordered_map>
#include <vector>
#include <memory>

namespace stride {
    class ContactPool;

    class Population;
} // namespace stride

namespace geopop {

    class Location;

    template<typename Policy, typename... F>
    class GeoAggregator;

/**
 * A Geographic grid of simulation region contains Locations that in turn contain
 * an index to the ContactPools situated at that Location.
 */
    class GeoGrid {//: public LocationGrid<Location> {
    public:
        /// GeoGrid and associated Population.
        explicit GeoGrid(stride::Population *population);

        /// No copy constructor.
        GeoGrid(const GeoGrid &) = delete;

        /// No copy assignment.
        GeoGrid operator=(const GeoGrid &) = delete;

        /// Gets current size of Location storage.
        size_t size() const { return m_locations.size(); }

        using iterator       = typename std::vector<std::shared_ptr<Location>>::iterator;
        using const_iterator = typename std::vector<std::shared_ptr<Location>>::const_iterator;

        /// Iterator to first EnhancedCoordinate.
        iterator begin() { return m_locations.begin(); }

        /// Iterator to the end of the EnhancedCoordinate storage.
        iterator end() { return m_locations.end(); }

        /// Const Iterator to first EnhancedCoordinate.
        const_iterator cbegin() const { return m_locations.cbegin(); }

        /// Const iterator to the end of the EnhancedCoordinate storage.
        const_iterator cend() const { return m_locations.cend(); }

        /// Gets a Location by index, doesn't performs a range check.
        std::shared_ptr<Location>& operator[](size_t index) { return m_locations[index]; }

        /// Get the Population associated with this GeoGrid
        stride::Population *GetPopulation() const { return m_population; }

        /// Find contactpools in startRadius (in km) around start and, if none are found, double
        /// the radius and search again until the radius gets infinite. May return an empty vector
        /// when there are really no pools to be found (empty grid).
        std::vector<stride::ContactPool *> GetNearbyPools(stride::ContactType::Id id,
                                                          const EnhancedCoordinate &start,
                                                          double startRadius = 10.0) const;

        /// Gets the K biggest (in population count) EnhancedCoordinates of this GeoGrid
        std::vector<Location *> TopK(size_t k) const {

            auto cmp = [](Location *rhs, Location *lhs) {
                return rhs->GetPopCount() > lhs->GetPopCount();
            };

            std::priority_queue<Location *, std::vector<Location *>, decltype(cmp)> queue(
                    cmp);
            for (const auto &loc : m_locations) {
                    queue.push(loc.get());
                    if (queue.size() > k) {
                    queue.pop();
                }
            }

            std::vector<Location *> topLocations;
            while (!queue.empty()) {
                auto loc = queue.top();
                topLocations.push_back(loc);
                queue.pop();
            }

            return topLocations;
        };

        void addLocation(std::shared_ptr<Location> loc, std::shared_ptr<EnhancedCoordinate> coor);

        /// Gets a Location by Id and check if the Id exists.
        std::shared_ptr<Location> GetById(unsigned int id) const {
            return m_locations[m_id_to_index.at(id)];
        }

        std::shared_ptr<LocationGrid<Location>> m_locationGrid;

    private:

        ///< Container for LocationData in GeoGrid.
        std::vector<std::shared_ptr<Location>> m_locations;

        ///< Stores pointer to Popluation, but does not take ownership.
        stride::Population *m_population;


        ///< Associative container maps Location Id to index in m_locations.
        std::unordered_map<unsigned int, unsigned int> m_id_to_index;
    };

} // namespace geopop
