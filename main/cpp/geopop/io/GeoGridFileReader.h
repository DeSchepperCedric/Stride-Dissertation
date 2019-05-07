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
 * An abstract base class for creating a GeoGrid that was read from a file, can be implemented
 * using multiple file types (proto and json are currently implemented)
 */
class GeoGridFileReader : public GeoGridReader
{
public:
        /// Parametrized constructor.
        explicit GeoGridFileReader(const std::string& inputFile, stride::Population* pop) : GeoGridReader(pop), m_inputFile(inputFile) {};

        /// No copy constructor.
        GeoGridFileReader(const GeoGridFileReader&) = delete;

        /// No copy assignment.
        GeoGridFileReader& operator=(const GeoGridFileReader&) = delete;

        /// Default destructor.
        virtual ~GeoGridFileReader() = default;

        /// Perform the actual read and return the created GeoGrid.
        virtual void Read() override = 0;

protected:
        std::string m_inputFile; ///< File to read.
};

} // namespace geopop
