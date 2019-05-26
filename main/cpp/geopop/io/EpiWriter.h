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

#include <memory>
#include <ostream>
#include <vector>

#include "../../../../qt/location.h"

namespace geopop {

class GeoGrid;

/**
 * An interface for writing the Epidemiologic information to a file, can be implemented with multiple file types.
 * are currently implemented.
 */
class EpiWriter
{
public:
        /// Construct the Writer.
        virtual ~EpiWriter() = default;

        /// Write the GeoGrid to ostream.
        virtual void Write(std::map<int, visualization::Location> locations) = 0;
};

} // namespace geopop
