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

#include "EpiFileWriter.h"
#include "geopop/EnhancedCoordinate.h"
#include "geopop/Location.h"

#include "H5Cpp.h"
#include <set>
#include <string>

namespace stride {
class ContactPool;
class Person;
} // namespace stride

namespace geopop {
/**
 * Writes a GeoGrid to a JSON file.
 */
class EpiHDF5Writer : public EpiFileWriter
{
public:
        explicit EpiHDF5Writer(const std::string& fileName);

        /// Write the provided GeoGrid to the proved ostream in JSON format.
        void Write(std::vector<geopop::EnhancedCoordinate> locations) override;

private:
        void WriteAttribute(const std::string& data, const std::string& name, H5::H5Object& object);

        /// Write an attribute of size 1 for data to a group or dataset
        template <typename T>
        void WriteAttribute(const T& data, const std::string& name, H5::H5Object& object);

        /// Create a HDF5 containing all info needed to reconstruct a Coordinate.
        void WriteCoordinate(const Coordinate& coordinate, H5::Group& location);

        /// Create a HDF5 Group containing all info needed to reconstruct a Location.
        void WriteLocation(const geopop::EnhancedCoordinate& location, H5::Group& locations,
                           const std::string& location_name);
};
} // namespace geopop
