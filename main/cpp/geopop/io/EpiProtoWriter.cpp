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

#include "EpiProtoWriter.h"

#include "epi.pb.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"
#include "util/Exception.h"
#include "util/SegmentedVector.h"

#include <iostream>
#include <map>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride::util;
using namespace stride::ContactType;

EpiProtoWriter::EpiProtoWriter(std::ostream& stream) : EpiStreamWriter(stream){}

void EpiProtoWriter::Write(std::vector<geopop::EnhancedCoordinate> locations)
{
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        proto::Epi protoGrid;
        for (auto& location : locations) {
                WriteLocation(location, protoGrid.add_locations());
        }
        cout << 2 << endl;
        if (!protoGrid.SerializeToOstream(&StreamRef())) {
                throw stride::util::Exception("There was an error writing the EpiData to the file.");
        }
        google::protobuf::ShutdownProtobufLibrary();
        StreamRef().flush();
}


void EpiProtoWriter::WriteCoordinate(const Coordinate&                   coordinate,
                                         proto::Epi_Location_Coordinate* protoCoordinate)
{
        protoCoordinate->set_longitude(boost::geometry::get<1>(coordinate));
        protoCoordinate->set_latitude(boost::geometry::get<0>(coordinate));
}

void EpiProtoWriter::WriteLocation(EnhancedCoordinate& location,
                                       proto::Epi_Location*             protoLocation)
{
        protoLocation->set_id(location.getData<visualization::Location>()->id);
        protoLocation->set_name(location.getData<visualization::Location>()->name);
        protoLocation->set_population(location.getData<visualization::Location>()->size);
        auto coordinate = new proto::Epi_Location_Coordinate();
        WriteCoordinate(location.GetCoordinate(), coordinate);
        protoLocation->set_allocated_coordinate(coordinate);

        auto epi = new proto::Epi_Location_Data();
        auto &infected = location.getData<visualization::Location>()->infected;
        for(auto &a: infected){
            auto age = epi->add_age();
            age->set_name(a.first);
            for(auto &s: a.second){
                auto status = age->add_status();
                status->set_name(s.first);
                for(auto& d: s.second){
                    status->add_day(d);
                }
            }
        }
        protoLocation->set_allocated_data(epi);
}

} // namespace geopop
