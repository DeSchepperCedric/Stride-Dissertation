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

#include <memory>

#include "Coordinate.h"

namespace geopop {
class EnhancedCoordinate
{
public:
        explicit EnhancedCoordinate(void* data, Coordinate coordinate = Coordinate(0.0, 0.0))
            : m_coordinate(coordinate), m_data(data)
        {
        }

        ~EnhancedCoordinate() = default;

        /// Sets the Coordinate of this Location.
        void SetCoordinate(const Coordinate& coordinate) { m_coordinate = coordinate; }

        /// Gets the Coordinate of this Location.
        const Coordinate GetCoordinate() const { return m_coordinate; }

        template <class T>
        void setData(T* data)
        {
                m_data = static_cast<void*>(data);
        }

        template <class T>
        T* getData() const
        {
                return static_cast<T*>(m_data);
        }

        bool operator==(const EnhancedCoordinate& coord)const{
            using boost::geometry::get;
            return get<0>(coord.m_coordinate) == get<0>(coord.m_coordinate) && get<1>(coord.m_coordinate) == get<1>(coord.m_coordinate);
        }

private:
        geopop::Coordinate  m_coordinate; ///< Coordinate of the Location.
        void*               m_data;       ///< Extra data for this coordinate
};

} // namespace geopop
