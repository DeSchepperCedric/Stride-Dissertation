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

#include "GeoGridJSONReader.h"

#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <memory>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

GeoGridJSONReader::GeoGridJSONReader(unique_ptr<istream> inputStream, Population* pop)
    : GeoGridStreamReader(move(inputStream), pop)
{
}

void GeoGridJSONReader::Read()
{
        nlohmann::json root;
        try {
                *m_inputStream >> root;
        } catch (nlohmann::json::parse_error& e) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        } /*catch (runtime_error&) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        }*/

        auto& geoGrid = m_population->RefGeoGrid();
        // WARNING: fails if "persons" contains empty string instead of empty array
        auto people = ParseArray(root.at("persons"));

        for (auto it = people.begin(); it != people.end(); it++) {
                auto person               = ParsePerson(*it);
                m_people[person->GetId()] = person;
        }
        auto locations = ParseArray(root.at("locations"));
        for (auto it = locations.begin(); it != locations.end(); it++) {
                shared_ptr<Location> loc;
                loc = ParseLocation(*it);
                geoGrid.AddLocation(move(loc));
        }

        AddCommutes(geoGrid);
        m_commutes.clear();
        m_people.clear();
}

shared_ptr<Location> GeoGridJSONReader::ParseLocation(nlohmann::json& location)
{
        const auto id         = ParseNumerical<unsigned int>(location.at("id"));
        const auto name       = location.at("name").get<std::string>();
        const auto province   = ParseNumerical<unsigned int>(location.at("province"));
        const auto population = ParseNumerical<unsigned int>(location.at("population"));
        const auto coordinate = ParseCoordinate(location.at("coordinate"));

        auto result         = make_shared<Location>(id, province, coordinate, name, population);
        auto contactPoolsClasses = ParseArray(location.at("contactPools"));

        for (auto it = contactPoolsClasses.begin(); it != contactPoolsClasses.end(); it++) {
                ParseContactPoolsClass(*it, result);
        }

        if (location.count("commutes")) {
                nlohmann::json commutes = ParseArray(location.at("commutes"));
                for (auto it = commutes.begin(); it != commutes.end(); it++) {
                        const auto to     = boost::lexical_cast<unsigned int>(it.key());
                        const auto amount = ParseNumerical<double>(*it);

                        m_commutes.emplace_back(id, to, amount);
                }
        }

        return result;
}

Coordinate GeoGridJSONReader::ParseCoordinate(nlohmann::json& coordinate)
{
        const auto longitude = ParseNumerical<double>(coordinate.at("longitude"));
        const auto latitude  = ParseNumerical<double>(coordinate.at("latitude"));
//        TODO worden die x en y gebruikt en wat is hier het praktisch nut van?
//        const auto x         = ParseNumerical<double>(coordinate.at("x"));
//        const auto y         = ParseNumerical<double>(coordinate.at("y"));
        return {longitude, latitude};
}

void GeoGridJSONReader::ParseContactPoolsClass(nlohmann::json& contactPoolClass, shared_ptr<Location> loc)
{
        const auto type = contactPoolClass.at("class").get<std::string>();
//        TODO check if this ID is deprecated?
//        const auto id   = ParseNumerical<unsigned int>(contactPoolClass.at("id"));

        ContactType::Id typeId;
        if (type == ToString(Id::K12School)) {
                typeId = Id::K12School;
        } else if (type == ToString(Id::College)) {
                typeId = Id::College;
        } else if (type == ToString(Id::Household)) {
                typeId = Id::Household;
        } else if (type == ToString(Id::PrimaryCommunity)) {
                typeId = Id::PrimaryCommunity;
        } else if (type == ToString(Id::SecondaryCommunity)) {
                typeId = Id::SecondaryCommunity;
        } else if (type == ToString(Id::Workplace)) {
                typeId = Id::Workplace;
        } else if (type == ToString(Id::Daycare)) {
                typeId = Id::Daycare;
        } else if (type == ToString(Id::PreSchool)) {
                typeId = Id::PreSchool;
        } else {
                throw Exception("No such ContactPool type: " + type);
        }

//        auto result       = make_shared<ContactPool>(id, typeId);
        auto contactPools = ParseArray(contactPoolClass.at("pools"));

        for (auto it = contactPools.begin(); it != contactPools.end(); it++) {
                ParseContactPool(loc, *it, typeId);
        }
}

void GeoGridJSONReader::ParseContactPool(shared_ptr<Location> loc, nlohmann::json& contactPool, ContactType::Id typeId)
{
        // Don't use the id of the ContactPool but the let the Population create an id.
        //TODO check
//        auto contactPool_id = ParseNumerical<unsigned int>(contactPool.at("id"));
        auto result = m_population->RefPoolSys().CreateContactPool(typeId);

        loc->RefPools(typeId).emplace_back(result);
        auto people = ParseArray(contactPool.at("people"));
        for (auto it = people.begin(); it != people.end(); it++) {
                auto       person_id = ParseNumerical<unsigned int>(*it);
                try {
                        const auto person    = m_people.at(person_id);
                        result->AddMember(person);
                        person->SetPoolId(typeId, result->GetId());
                } catch (std::out_of_range& e){
                        throw Exception("No such person with id: " + person_id);
                }
        }
}

Person* GeoGridJSONReader::ParsePerson(nlohmann::json& person)
{
        const auto id   = ParseNumerical<unsigned int>(person.at("id"));
        const auto age  = ParseNumerical<unsigned int>(person.at("age"));
        // The following ID's of contactpools will not necessarily coincide with the actual ID of the contactpool, due to how ID's are generated
        // This issue will be fixed later on, when parsing the contactpools, then the correct contactpool ID will be assigned.
        const auto hhId = ParseNumerical<unsigned int>(person.at("Household"));
        const auto ksId = ParseNumerical<unsigned int>(person.at("K12School"));
        const auto coId = ParseNumerical<unsigned int>(person.at("College"));
        const auto wpId = ParseNumerical<unsigned int>(person.at("Workplace"));
        const auto pcId = ParseNumerical<unsigned int>(person.at("PrimaryCommunity"));
        const auto scId = ParseNumerical<unsigned int>(person.at("SecondaryCommunity"));
        const auto dcId = ParseNumerical<unsigned int>(person.at("Daycare"));
        const auto psId = ParseNumerical<unsigned int>(person.at("PreSchool"));

        return m_population->CreatePerson(id, age, hhId, ksId, coId, wpId, pcId, scId, dcId, psId);
}

nlohmann::json GeoGridJSONReader::ParseArray(nlohmann::json& node)
{
    if (node.type() == nlohmann::json::value_t::string) {
        return nlohmann::json::array();
    } else {
        return node;
    }
}

} // namespace geopop
