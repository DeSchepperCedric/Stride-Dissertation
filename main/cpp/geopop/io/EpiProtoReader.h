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

#include "EpiStreamReader.h"
#include "geopop/EnhancedCoordinate.h"
#include "geopop/Location.h"

#include <memory>
#include <set>

namespace proto {
class Epi_Location;
class Epi_Location_Coordinate;
} // namespace proto

namespace geopop {

using namespace std;

/**
 * An implementation of the GeoGridReader using Protocol Buffers.
 * This class is used to read a GeoGrid from a Proto file.
 */
class EpiProtoReader : public EpiStreamReader
{
public:
        /// Construct the GeoGridJSONReader with the istream which contains the Protobuf info.
        EpiProtoReader(std::unique_ptr<std::istream> inputStream);

        /// No copy constructor.
        EpiProtoReader(const EpiProtoReader&) = delete;

        /// No copy assignement.
        EpiProtoReader operator=(const EpiProtoReader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        std::pair<std::vector<visualization::Location*>, std::vector<geopop::EnhancedCoordinate>> Read() override;

private:
        /// Create a Coordinate based on the provided protobuf Coordinate.
        Coordinate ParseCoordinate(const proto::Epi_Location_Coordinate& protoCoordinate);

        /// Create a Location based on  protobuf Location info.
        std::pair<visualization::Location*, geopop::EnhancedCoordinate> ParseLocation(
            const proto::Epi_Location& protoLocation);
};

} // namespace geopop
