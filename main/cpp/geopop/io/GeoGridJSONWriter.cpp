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

#include "GeoGridJSONWriter.h"

//#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"

//#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

GeoGridJSONWriter::GeoGridJSONWriter() : m_persons_found() {}

void GeoGridJSONWriter::Write(GeoGrid& geoGrid, ostream& stream)
{
        nlohmann::json root;
        nlohmann::json locations = nlohmann::json::array();

        for (const auto& location : geoGrid) {
                nlohmann::json child;
                child = WriteLocation(*location);
                locations.push_back(move(child));
        }

        root["locations"] = locations;

        nlohmann::json persons = nlohmann::json::array();

        for (const auto& person : m_persons_found) {
                nlohmann::json child;
                child = WritePerson(person);
                persons.push_back(move(child));
        }
        root["persons"] = persons;

        m_persons_found.clear();

        stream << root;
}

nlohmann::json GeoGridJSONWriter::WriteContactCenter(ContactPool* const & contactPool)
{
        nlohmann::json pools = nlohmann::json::array();
        for (auto pool : *contactPool){
                nlohmann::json child;
                nlohmann::json people = nlohmann::json::array();
                child["id"] = contactPool->GetId();
//                for (auto person : pool{
                        auto person = pool;
                        m_persons_found.insert(person);
                        people.push_back(person->GetId());
//                }
                child["people"] = people;
                pools.push_back(move(child));
        }
        return pools;
}

nlohmann::json GeoGridJSONWriter::WriteCoordinate(const Coordinate& coordinate)
{
        nlohmann::json coordinate_root;
        coordinate_root["longitude"] = boost::geometry::get<0>(coordinate);
        coordinate_root["latitude"] = boost::geometry::get<1>(coordinate);
        return coordinate_root;
}

nlohmann::json GeoGridJSONWriter::WriteLocation(const Location& location)
{
        nlohmann::json location_root;
        location_root["id"] = location.GetID();
        location_root["name"] = location.GetName();
        location_root["province"] = location.GetProvince();
        location_root["population"] = location.GetPopCount();
        location_root["coordinate"] = WriteCoordinate(location.GetCoordinate());

        auto commutes = location.CRefOutgoingCommutes();
        if (!commutes.empty()) {
                nlohmann::json commutes_root;
                for (auto commute_pair : commutes) {
                        commutes_root[to_string(commute_pair.first->GetID())] = commute_pair.second;
                }
                location_root["commutes"] = commutes_root;
        }

        nlohmann::json contactCenters = nlohmann::json::array();
        for (Id typ : IdList) {
                for (const auto& c : location.CRefPools(typ)) {
                        nlohmann::json child;
                        child["type"] = ToString(typ);
                        child["id"] = c->GetId();
                        child = WriteContactCenter(c);
                        contactCenters.push_back(move(child));
                }
        }
        location_root["contactCenters"] = contactCenters;

        return location_root;
}

nlohmann::json GeoGridJSONWriter::WritePerson(Person* person)
{
        using namespace ContactType;

        nlohmann::json person_root;
        person_root["id"] = person->GetId();
        person_root["age"] = person->GetAge();
        person_root["K12School"] = person->GetPoolId(Id::K12School);
        person_root["College"] = person->GetPoolId(Id::College);
        person_root["Household"] = person->GetPoolId(Id::Household);
        person_root["Workplace"] = person->GetPoolId(Id::Workplace);
        person_root["PrimaryCommunity"] = person->GetPoolId(Id::PrimaryCommunity);
        person_root["SecondaryCommunity"] = person->GetPoolId(Id::SecondaryCommunity);
        person_root["Daycare"] = person->GetPoolId(Id::Daycare);
        person_root["PreSchool"] = person->GetPoolId(Id::PreSchool);
        return person_root;
}

} // namespace geopop
