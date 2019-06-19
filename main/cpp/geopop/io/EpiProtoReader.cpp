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
 *  Copyright 2018, 2019Jan Broeckhove and Bistromatics group.
 */

#include "EpiProtoReader.h"

#include "epi.pb.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"
#include "pop/Population.h"

#include <iostream>
#include <stdexcept>

namespace geopop {

using namespace std;
using namespace stride::ContactType;

EpiProtoReader::EpiProtoReader(unique_ptr<istream> inputStream)
    : EpiStreamReader(move(inputStream))
{
}

std::pair<std::vector<visualization::Location*>, std::vector<geopop::EnhancedCoordinate>> EpiProtoReader::Read()
{
        proto::Epi protoGrid;
        if (!protoGrid.ParseFromIstream(m_inputStream.get())) {
                throw runtime_error("Failed to parse Proto file");
        }

        std::vector<visualization::Location*>   locs;
        std::vector<geopop::EnhancedCoordinate> coords;


        for (int idx = 0; idx < protoGrid.locations_size(); idx++) {
                    const proto::Epi_Location& protoLocation = protoGrid.locations(idx);
                    auto                           loc           = ParseLocation(protoLocation);
                locs.push_back(loc.first);
                coords.push_back(loc.second);
            }
    return make_pair(locs, coords);
}

Coordinate EpiProtoReader::ParseCoordinate(const proto::Epi_Location_Coordinate& protoCoordinate)
{
        return {protoCoordinate.longitude(), protoCoordinate.latitude()};
}

std::pair<visualization::Location*, geopop::EnhancedCoordinate> EpiProtoReader::ParseLocation(
    const proto::Epi_Location& protoLocation)
{

        const auto  id         = protoLocation.id();
        const auto& name       = protoLocation.name();
        const auto  population = protoLocation.population();
        const auto& coordinate = ParseCoordinate(protoLocation.coordinate());

        geopop::EnhancedCoordinate coord(nullptr);
        visualization::Location*   loc = new visualization::Location();

        coord.SetCoordinate(coordinate);
        coord.setData(loc);

        loc->id = id;
        loc->name = name;
        loc->size = population;

        const auto age    = {"College",   "Daycare",          "Household",          "K12School",
                             "PreSchool", "PrimaryCommunity", "SecondaryCommunity", "Workplace"};
        const auto status = {"immune", "infected", "infectious", "recovered", "susceptible", "symptomatic", "total"};

        for (const auto& a : protoLocation.data().age()) {
                for (const auto& s : a.status()) {
                        vector<unsigned int> days;
                        for(const auto& d: s.day()){
                                days.push_back(d);
                        }
                        loc->infected[a.name()][s.name()]       = days;
                }
        }



        return std::make_pair(loc, coord);
}

} // namespace geopop
