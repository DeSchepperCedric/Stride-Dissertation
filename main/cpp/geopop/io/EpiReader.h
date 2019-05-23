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

#include "../../../../qt/location.h"
#include <map>
#include <memory>
#include <tuple>
#include <vector>

namespace stride {
class Person;
class Population;
} // namespace stride

namespace geopop {

class GeoGrid;

/**
 * An abstract base class for creating a GeoGrid that was read from a file, can be implemented
 * using multiple file types (proto and json are currently implemented)
 */
class EpiReader
{
public:
        EpiReader() = default;

        /// No copy constructor.
        EpiReader(const EpiReader&) = delete;

        /// No copy assignment.
        EpiReader& operator=(const EpiReader&) = delete;

        /// Default destructor.
        virtual ~EpiReader() = default;

        /// Perform the actual read and return the created vector<Location>.
        virtual std::vector<visualization::Location> Read() = 0;
};

} // namespace geopop
