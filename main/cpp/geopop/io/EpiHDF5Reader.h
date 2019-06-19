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

#include "../EnhancedCoordinate.h"
#include "EpiFileReader.h"

#include "H5Cpp.h"
#include "util/HDF5.h"
#include <boost/lexical_cast.hpp>
#include <geopop/Location.h>

#include "../../../../qt/location.h"

namespace geopop {

class GeoGrid;

/**
 * An implementation of the GeoGridReader using JSON.
 * This class is used to read a GeoGrid from a JSON file.
 */
class EpiHDF5Reader : public EpiFileReader
{
public:
        /// Construct the GeoGridJSONReader with the istream which contains the JSON.
        explicit EpiHDF5Reader(std::string inputFile);

        /// No copy constructor.
        EpiHDF5Reader(const EpiHDF5Reader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        std::pair<std::vector<visualization::Location*>, std::vector<geopop::EnhancedCoordinate>> Read() override;

private:
        template <typename T>
        T ReadAttribute(const std::string& name, H5::H5Object& object);

        std::pair<visualization::Location*, EnhancedCoordinate> ParseLocation(H5::Group& location);
};

} // namespace geopop
