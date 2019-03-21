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

#include "GeoGridJSONWriter.h"

#include "contact/ContactPool.h"
#include "geopop/ContactCenter.h"
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

#pragma omp parallel
#pragma omp single
        {
                for (const auto& location : geoGrid) {
                        nlohmann::json child;
#pragma omp task firstprivate(location)
                        {
                                child = WriteLocation(location);
#pragma omp critical

                                locations.push_back(move(child));
                        }
                }
#pragma omp taskwait
        }

        root["locations"] = locations;

        nlohmann::json persons = nlohmann::json::array();
#pragma omp parallel
#pragma omp single
        {
                for (const auto& person : m_persons_found) {
                        nlohmann::json child;
#pragma omp task firstprivate(person)
                        {
                                child = WritePerson(person);
#pragma omp critical
                                persons.push_back(move(child));
                        }
                }
#pragma omp taskwait
        }
        root["persons"] = persons;

        m_persons_found.clear();
        stream << root;
}

nlohmann::json GeoGridJSONWriter::WriteContactCenter(shared_ptr<ContactCenter> contactCenter)
{
        nlohmann::json contactCenter_root;
        contactCenter_root["id"] = contactCenter->GetId();
        contactCenter_root["type"] = ToString(contactCenter->GetContactPoolType());
        nlohmann::json pools = nlohmann::json::array();
#pragma omp parallel
#pragma omp single
        {
                for (const auto& pool : *contactCenter) {
                        nlohmann::json child;
#pragma omp task firstprivate(pool)
                        {
                                child = WriteContactPool(pool);
#pragma omp critical
                                pools.push_back(move(child));
                        }
                }
#pragma omp taskwait
        }
        contactCenter_root["pools"] = pools;
        return contactCenter_root;
}

nlohmann::json GeoGridJSONWriter::WriteContactPool(ContactPool* contactPool)
{
        // contactPool_root is going to be an element in the "pools" array
        nlohmann::json contactPool_root;
        contactPool_root["id"] = contactPool->GetId();
        // people is an array of ID's
        nlohmann::json people = nlohmann::json::array();
        for (auto person : *contactPool) {
#pragma omp critical
                m_persons_found.insert(person);
                people.push_back(person->GetId());
        }
        contactPool_root["people"] = people;
        return contactPool_root;
}

nlohmann::json GeoGridJSONWriter::WriteCoordinate(const Coordinate& coordinate)
{
        nlohmann::json coordinate_root;
        coordinate_root["longitude"] = boost::geometry::get<0>(coordinate);
        coordinate_root["latitude"] = boost::geometry::get<1>(coordinate);
        return coordinate_root;
}

nlohmann::json GeoGridJSONWriter::WriteLocation(shared_ptr<Location> location)
{
        nlohmann::json location_root;
        location_root["id"] = location->GetID();
        location_root["name"] = location->GetName();
        location_root["province"] = location->GetProvince();
        location_root["population"] = location->GetPopCount();
        location_root["coordinate"] = WriteCoordinate(location->GetCoordinate());

        auto commutes = location->CRefOutgoingCommutes();
        if (!commutes.empty()) {
                nlohmann::json commutes_root;
                for (auto commute_pair : commutes) {
                        commutes_root[to_string(commute_pair.first->GetID())] = commute_pair.second;
                }
                location_root["commutes"] = commutes_root;
        }

        nlohmann::json                    contactCenters = nlohmann::json::array();
        vector<shared_ptr<ContactCenter>> centers;
        for (Id typ : IdList) {
                for (const auto& c : location->RefCenters(typ)) {
                        nlohmann::json child;
                        {
                                child = WriteContactCenter(c);
                                contactCenters.push_back(move(child));
                        }
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
        return person_root;
}

} // namespace geopop
