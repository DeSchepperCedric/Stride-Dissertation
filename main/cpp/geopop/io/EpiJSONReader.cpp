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

#include "EpiJSONReader.h"

#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <iostream>
#include <memory>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

EpiJSONReader::EpiJSONReader(unique_ptr<istream> inputStream) : EpiStreamReader(move(inputStream)) {}

std::vector<visualization::Location> EpiJSONReader::Read()
{
        nlohmann::json root;
        try {
                *m_inputStream >> root;
        } catch (nlohmann::json::parse_error& e) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        } /*catch (runtime_error&) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        }*/

        std::vector<visualization::Location> locs;

        auto locations = ParseArray(root.at("locations"));
        for (auto it = locations.begin(); it != locations.end(); it++) {
                visualization::Location loc = parseLocation(*it);
                locs.push_back(loc);
        }
        return locs;
}

visualization::Location EpiJSONReader::parseLocation(nlohmann::json& node)
{
        visualization::Location loc;

        loc.size      = node["population"];
        loc.name      = node["name"];
        loc.id        = node["id"];
        loc.latitude  = double(node["coordinate"]["latitude"]);
        loc.longitude = double(node["coordinate"]["longitude"]);


        const auto age = {"College", "Daycare", "Household", "K12School", "PreSchool", "PrimaryCommunity", "SecondaryCommunity", "Workplace"};
        const auto status = {"immune", "infected", "infectious", "recovered", "susceptible", "symptomatic", "total"};

        
        for(const auto& a: age){
            for(const auto& s: status){
                vector<unsigned int> days = ParseArray(node["Epi"][a][s]);
                loc.infected[a][s] = days;
            }
        }


        return loc;
}

nlohmann::json EpiJSONReader::ParseArray(nlohmann::json& node)
{
        if (node.type() == nlohmann::json::value_t::string) {
                return nlohmann::json::array();
        } else {
                return node;
        }
}

} // namespace geopop
