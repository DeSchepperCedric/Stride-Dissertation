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

#include "ThreadException.h"
#include "geopop/CollegeCenter.h"
#include "geopop/GeoGrid.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/K12SchoolCenter.h"
#include "geopop/PrimaryCommunityCenter.h"
#include "geopop/SecondaryCommunityCenter.h"
#include "geopop/WorkplaceCenter.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <memory>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

GeoGridJSONReader::GeoGridJSONReader(unique_ptr<istream> inputStream, Population* pop)
    : GeoGridReader(move(inputStream), pop)
{
}

void GeoGridJSONReader::Read()
{
    nlohmann::json root;
        try {
                *m_inputStream >> root;
        } catch (runtime_error&) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        }

        auto& geoGrid = m_population->RefGeoGrid();
        // WARNING: fails if "persons" contains empty string instead of empty array
        auto people  = root.at("persons");
        if (people != ""){

#pragma omp parallel
#pragma omp single
            {
                for (auto it = people.begin(); it != people.end(); it++) {
#pragma omp task firstprivate(it)
                    {
#pragma omp critical
                        {
                            auto person = ParsePerson(*it);
                            m_people[person->GetId()] = person;
                        }
                    }
                }
#pragma omp taskwait
            }
        }
        auto locations = root.at("locations");
        auto e         = make_shared<ThreadException>();
#pragma omp parallel
#pragma omp single
        {
                for (auto it = locations.begin(); it != locations.end(); it++) {
                        shared_ptr<Location> loc;
#pragma omp task firstprivate(it, loc)
                        {
                                e->Run([&loc, this, &it] { loc = ParseLocation(*it); });
                                if (!e->HasError())
#pragma omp critical
                                        geoGrid.AddLocation(move(loc));
                        }
                }
#pragma omp taskwait
        }
        e->Rethrow();
        AddCommutes(geoGrid);
        m_commutes.clear();
        m_people.clear();
}

shared_ptr<Location> GeoGridJSONReader::ParseLocation(nlohmann::json& location)
{
/*        const auto id         = location.at("id").get<unsigned int>();
        const auto name       = location.at("name").get<std::string>();
        const auto province   = location.at("province").get<unsigned int>();
        const auto population = location.at("population").get<unsigned int>();
*/
        const auto id         = boost::lexical_cast<unsigned int>(location.at("id").get<std::string>());
        const auto name       = location.at("name").get<std::string>();
        const auto province   = boost::lexical_cast<unsigned int>(location.at("province").get<std::string>());
        const auto population = boost::lexical_cast<unsigned int>(location.at("population").get<std::string>());
        const auto coordinate = ParseCoordinate(location.at("coordinate"));

        auto result         = make_shared<Location>(id, province, coordinate, name, population);
        auto contactCenters = location.at("contactCenters");
        auto e              = make_shared<ThreadException>();

        if (contactCenters != "") {

#pragma omp parallel
#pragma omp single
            {
                for (auto it = contactCenters.begin(); it != contactCenters.end(); it++) {
                    shared_ptr<ContactCenter> center;
#pragma omp task firstprivate(it, center)
                    {
                        e->Run([&it, this, &center] { center = ParseContactCenter(*it); });
                        if (!e->HasError())
#pragma omp critical
                            result->AddCenter(center);
                    }
                }
#pragma omp taskwait
            }
        }
        e->Rethrow();

        if (location.count("commutes")) {
                nlohmann::json commutes = location.at("commutes");
                for (auto it = commutes.begin(); it != commutes.end(); it++) {
                        const auto to     = boost::lexical_cast<unsigned int>(it.key());
                    const auto amount = boost::lexical_cast<double>(it->get<std::string>());
                    //const auto amount = it->get<double>();

#pragma omp critical
                        m_commutes.emplace_back(id, to, amount);
                }
        }

        return result;
}

Coordinate GeoGridJSONReader::ParseCoordinate(nlohmann::json& coordinate)
{
//        const auto longitude = coordinate.at("longitude").get<double>();
//        const auto latitude  = coordinate.at("latitude").get<double>();
        const auto longitude = boost::lexical_cast<double>(coordinate.at("longitude").get<std::string>());
        const auto latitude  = boost::lexical_cast<double>(coordinate.at("latitude").get<std::string>());
        return {longitude, latitude};
}

shared_ptr<ContactCenter> GeoGridJSONReader::ParseContactCenter(nlohmann::json& contactCenter)
{
        const auto type = contactCenter.at("type").get<std::string>();
//        const auto id   = contactCenter.at("id").get<unsigned int>();
        const auto id   = boost::lexical_cast<unsigned int>(contactCenter.at("id").get<std::string>());

        shared_ptr<ContactCenter> result;
        ContactType::Id           typeId;
        if (type == ToString(Id::K12School)) {
                result = make_shared<K12SchoolCenter>(id);
                typeId = Id::K12School;
        } else if (type == ToString(Id::College)) {
                result = make_shared<CollegeCenter>(id);
                typeId = Id::College;
        } else if (type == ToString(Id::Household)) {
                result = make_shared<HouseholdCenter>(id);
                typeId = Id::Household;
        } else if (type == ToString(Id::PrimaryCommunity)) {
                result = make_shared<PrimaryCommunityCenter>(id);
                typeId = Id::PrimaryCommunity;
        } else if (type == ToString(Id::SecondaryCommunity)) {
                result = make_shared<SecondaryCommunityCenter>(id);
                typeId = Id::SecondaryCommunity;
        } else if (type == ToString(Id::Workplace)) {
                result = make_shared<WorkplaceCenter>(id);
                typeId = Id::Workplace;
        } else {
                throw Exception("No such ContactCenter type: " + type);
        }

        auto contactPools = contactCenter.at("pools");
        auto e            = make_shared<ThreadException>();

        if (contactPools != "") {

#pragma omp parallel
#pragma omp single
            {
                for (auto it = contactPools.begin(); it != contactPools.end(); it++) {
#pragma omp task firstprivate(it)
                    {
                        ContactPool *pool = nullptr;
                        e->Run([&it, &pool, this, typeId] {
                            pool = ParseContactPool(*it, typeId);
                        });
                        if (!e->HasError()) {
#pragma omp critical
                            result->RegisterPool(pool);
                        }
                    }
                }
#pragma omp taskwait
            }
        }
        e->Rethrow();
        return result;
}

ContactPool* GeoGridJSONReader::ParseContactPool(nlohmann::json& contactPool, ContactType::Id typeId)
{
        // Don't use the id of the ContactPool but the let the Population create an id.
        auto result = m_population->RefPoolSys().CreateContactPool(typeId);
        auto people = contactPool.at("people");
        if (people != "") {
            for (auto it = people.begin(); it != people.end(); it++) {
                auto person_id = boost::lexical_cast<unsigned int>(it->get<std::string>());
                if (m_people.count(person_id) == 0) {
                    throw Exception("No such person: " + to_string(person_id));
                }
#pragma omp critical
                result->AddMember(m_people[person_id]);
            }
        }

        return result;
}

Person* GeoGridJSONReader::ParsePerson(nlohmann::json& person)
{
/*        const auto id   = person.at("id").get<unsigned int>();
        const auto age  = person.at("age").get<unsigned int>();
        const auto hhId = person.at("Household").get<unsigned int>();
        const auto ksId = person.at("K12School").get<unsigned int>();
        const auto coId = person.at("College").get<unsigned int>();
        const auto wpId = person.at("Workplace").get<unsigned int>();
        const auto pcId = person.at("PrimaryCommunity").get<unsigned int>();
        const auto scId = person.at("SecondaryCommunity").get<unsigned int>();
*/
        const auto id   = boost::lexical_cast<unsigned int>(person.at("id").get<std::string>());
        const auto age  = boost::lexical_cast<unsigned int>(person.at("age").get<std::string>());
        const auto hhId = boost::lexical_cast<unsigned int>(person.at("Household").get<std::string>());
        const auto ksId = boost::lexical_cast<unsigned int>(person.at("K12School").get<std::string>());
        const auto coId = boost::lexical_cast<unsigned int>(person.at("College").get<std::string>());
        const auto wpId = boost::lexical_cast<unsigned int>(person.at("Workplace").get<std::string>());
        const auto pcId = boost::lexical_cast<unsigned int>(person.at("PrimaryCommunity").get<std::string>());
        const auto scId = boost::lexical_cast<unsigned int>(person.at("SecondaryCommunity").get<std::string>());

    return m_population->CreatePerson(id, age, hhId, ksId, coId, wpId, pcId, scId);
}

} // namespace geopop
