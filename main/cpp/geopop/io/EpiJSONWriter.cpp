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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "EpiJSONWriter.h"

#include <iostream>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace nlohmann;

EpiJSONWriter::EpiJSONWriter(std::ostream& stream) : EpiStreamWriter(stream) {}

void EpiJSONWriter::Write(std::vector<geopop::EnhancedCoordinate> locations)
{

        json root;
        json locs = json::array();

        for (auto& loc : locations) {
                nlohmann::json child;
                child = WriteLocation(loc);
                locs.push_back(move(child));
        }

        root["locations"] = locs;

        StreamRef() << root;
}

json EpiJSONWriter::WriteLocation(const geopop::EnhancedCoordinate& location)
{

        nlohmann::json location_root;
        location_root["id"]         = location.getData<visualization::Location>()->id;
        location_root["name"]       = location.getData<visualization::Location>()->name;
        location_root["population"] = location.getData<visualization::Location>()->size;
        json coordinate;
        coordinate["longitude"]     = location.GetCoordinate().get<0>();
        coordinate["latitude"]      = location.GetCoordinate().get<1>();
        location_root["coordinate"] = coordinate;
        json infected;
        for (const auto& age: location.getData<visualization::Location>()->infected){
            json statusJ;
            for (const auto& status: age.second){
                json dayJ = json::array();
                for (const auto& day: status.second){
                    dayJ.push_back(day);
                }
                statusJ[status.first] = dayJ;
            }
            infected[age.first] = statusJ;
        }
        location_root["Epi"] = infected;

        return location_root;
}

} // namespace geopop
