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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "GeoGridReader.h"

#include <istream>

namespace geopop {

class GeoGrid;

/**
 * An abstract class for creating a GeoGrid that was read from a file, can be implemented
 * using multiple file types (proto and json are currently implemented)
 */
class GeoGridStreamReader : public GeoGridReader
{
public:
        /// Parametrized constructor.
        explicit GeoGridStreamReader(std::unique_ptr<std::istream> inputStream, stride::Population* pop) : GeoGridReader(pop), m_inputStream(std::move(inputStream)) {};

        /// No copy constructor.
        GeoGridStreamReader(const GeoGridStreamReader&) = delete;

        /// No copy assignment.
        GeoGridStreamReader& operator=(const GeoGridStreamReader&) = delete;

        /// Default destructor.
        virtual ~GeoGridStreamReader() = default;

        /// Perform the actual read and return the created GeoGrid.
        virtual void Read() override = 0;

protected:
        std::unique_ptr<std::istream> m_inputStream; ///< File to read.
};

} // namespace geopop
