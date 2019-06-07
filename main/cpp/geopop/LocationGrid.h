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

#include <geopop/geo/GeoAggregator.h>
#include <geopop/geo/KdTree2DPoint.h>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

#include "EnhancedCoordinate.h"

namespace geopop {
template <typename Policy, typename... F>
class GeoAggregator;

template <typename T>
class LocationGrid
{
public:
        LocationGrid() : m_coordinates(), m_finalized(false), m_tree(){};

        ~LocationGrid() = default;

        /// Gets current size of Location storage.
        size_t size() const { return m_coordinates.size(); }

        using iterator       = typename std::vector<std::shared_ptr<EnhancedCoordinate>>::iterator;
        using const_iterator = typename std::vector<std::shared_ptr<EnhancedCoordinate>>::const_iterator;

        /// Iterator to first EnhancedCoordinate.
        iterator begin() { return m_coordinates.begin(); }

        /// Iterator to the end of the EnhancedCoordinate storage.
        iterator end() { return m_coordinates.end(); }

        /// Const Iterator to first EnhancedCoordinate.
        const_iterator cbegin() const { return m_coordinates.cbegin(); }

        /// Const iterator to the end of the EnhancedCoordinate storage.
        const_iterator cend() const { return m_coordinates.cend(); }

        /// Gets a EnhancedCoordinate by index, doesn't performs a range check.
        const std::shared_ptr<EnhancedCoordinate>& operator[](size_t index) const { return m_coordinates[index]; }

        /// Build a GeoAggregator with a predefined functor and given args for the Policy.
        template <typename Policy, typename F>
        GeoAggregator<Policy, F> BuildAggregator(F functor, typename Policy::Args&& args) const
        {
                return GeoAggregator<Policy, F>(m_tree, functor, std::forward<typename Policy::Args>(args));
        }

        /// Build a GeoAggregator that gets its functor when calling, with given args for the Policy.
        template <typename Policy>
        GeoAggregator<Policy> BuildAggregator(typename Policy::Args&& args) const
        {
                return GeoAggregator<Policy>(m_tree, std::forward<typename Policy::Args>(args));
        }

        /**
         * Gets the EnhancedCoordinates in a rectangle determined by the two coordinates (long1, lat1) and (long2,
         * lat2). The coordinates must be positioned on the diagonal, i.e:
         *
         *  p1 -----+     +-------p1
         *  |       |     |       |
         *  |       |  or |       |
         *  |       |     |       |
         *  +-------p2    p2------+
         */
        std::set<const EnhancedCoordinate*> LocationsInBox(double long1, double lat1, double long2, double lat2) const
        {
                CheckFinalized(__func__);

                std::set<const EnhancedCoordinate*> result;

                auto agg = BuildAggregator<BoxPolicy>(MakeCollector(inserter(result, result.begin())),
                                                      std::make_tuple(std::min(long1, long2), std::min(lat1, lat2),
                                                                      std::max(long1, long2), std::max(lat1, lat2)));
                agg();

                return result;
        }

        /// Gets the EnhancedCoordinates in a rectangle defined by the two Locations.
        std::set<const EnhancedCoordinate*> LocationsInBox(EnhancedCoordinate* loc1, EnhancedCoordinate* loc2) const
        {
                using boost::geometry::get;
                return LocationsInBox(get<0>(loc1->GetCoordinate()), get<1>(loc1->GetCoordinate()),
                                      get<0>(loc2->GetCoordinate()), get<1>(loc2->GetCoordinate()));
        }

        /// Search for locations in \p radius (in km) around \p start.
        std::vector<const EnhancedCoordinate*> LocationsInRadius(const EnhancedCoordinate& start, double radius) const
        {
                CheckFinalized(__func__);

                geogrid_detail::KdTree2DPoint          startPt(&start);
                std::vector<const EnhancedCoordinate*> result;

                auto agg = BuildAggregator<RadiusPolicy>(MakeCollector(back_inserter(result)),
                                                         std::make_tuple(startPt, radius));
                agg();

                return result;
        }

        void AddData(std::shared_ptr<EnhancedCoordinate> location)
        {
                if (m_finalized) {
                        throw std::runtime_error("Calling addLocation while GeoGrid is finalized not supported!");
                }
                m_coordinates.emplace_back(location);
                // TODO: fix having no ID
                // m_id_to_index[0] = static_cast<unsigned int>(m_coordinates.size() - 1);
        }

        void CheckFinalized(const std::string& functionName) const
        {
                if (!m_finalized) {
                        throw std::runtime_error("Calling \"" + functionName +
                                                 "\" with GeoGrid not finalized not supported!");
                }
        }

        void Finalize()
        {
                std::vector<geogrid_detail::KdTree2DPoint> points;
                for (const auto& loc : m_coordinates) {
                        points.emplace_back(geogrid_detail::KdTree2DPoint(loc.get()));
                }
                m_tree      = GeoGridKdTree::Build(points);
                m_finalized = true;
        }

protected:
        ///< Container for EnhancedCoordinates in GeoGrid.
        std::vector<std::shared_ptr<EnhancedCoordinate>> m_coordinates;

        ///< Is the GeoGrid finalized (ready for use) yet?
        bool m_finalized;

        ///< Internal KdTree for quick spatial lookup.
        GeoGridKdTree m_tree;
};

} // namespace geopop
