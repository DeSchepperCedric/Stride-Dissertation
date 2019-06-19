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

#include "EpiStreamWriter.h"
#include "geopop/EnhancedCoordinate.h"
#include "geopop/Location.h"

#include <memory>
#include <set>

namespace stride {
} // namespace stride

namespace proto {
class Epi_Location;
class Epi_Location_Coordinate;
class Epi_Location_Data;
class Epi_Location_Data_Age;
class Epi_Location_Data_Age_Status;
} // namespace proto

namespace geopop {

/**
 * An implementation of the GeoGridWriter using Protocol Buffers
 * This class is used to write a GeoGrid to a Proto file
 */
class EpiProtoWriter : public EpiStreamWriter
{
public:
        /// Construct the EpiProtoWriter.
        explicit EpiProtoWriter(std::ostream& stream);

        /// Write the GeoGrid to the ostream in Protobuf format.
        void Write(std::vector<geopop::EnhancedCoordinate> locations) override;

private:
        /// Create a ProtoBuf Coordinate containing all the info needed to reconstruct a Coordinate..
        void WriteCoordinate(const Coordinate& coordinate, proto::Epi_Location_Coordinate* protoCoordinate);

        /// Create a ProtoBuf Location containing all the info needed to reconstruct a Location.
        void WriteLocation(EnhancedCoordinate& location, proto::Epi_Location* protoLocation);
};

} // namespace geopop
