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

#include "GeoGridProtoReader.h"

#include "ThreadException.h"
#include "gengeopop/College.h"
#include "gengeopop/GeoGrid.h"
#include "gengeopop/Household.h"
#include "gengeopop/K12School.h"
#include "gengeopop/PrimaryCommunity.h"
#include "gengeopop/SecondaryCommunity.h"
#include "gengeopop/Workplace.h"
#include "pop/Person.h"
#include "proto/geogrid.pb.h"

#include <iostream>
#include <omp.h>
#include <stdexcept>

namespace gengeopop {

using namespace std;

GeoGridProtoReader::GeoGridProtoReader(unique_ptr<istream> inputStream, stride::Population* pop)
    : GeoGridReader(move(inputStream), pop), m_geoGrid()
{
}

shared_ptr<GeoGrid> GeoGridProtoReader::Read()
{
        proto::GeoGrid protoGrid;
        if (!protoGrid.ParseFromIstream(m_inputStream.get())) {
                throw runtime_error("Failed to parse Proto file");
        }
        m_geoGrid = make_shared<GeoGrid>(m_population);
#pragma omp parallel
#pragma omp single
        {
                for (int idx = 0; idx < protoGrid.persons_size(); idx++) {
                        const proto::GeoGrid_Person& protoPerson = protoGrid.persons(idx);
#pragma omp task firstprivate(protoPerson)
                        {
#pragma omp critical
                                {
                                        stride::Person* person    = ParsePerson(protoPerson);
                                        m_people[person->GetId()] = person;
                                }
                        }
                }
#pragma omp taskwait
        }
        auto e = make_shared<ThreadException>();
#pragma omp parallel
#pragma omp single
        {
                for (int idx = 0; idx < protoGrid.locations_size(); idx++) {
                        shared_ptr<Location>           loc;
                        const proto::GeoGrid_Location& protoLocation = protoGrid.locations(idx);
#pragma omp task firstprivate(protoLocation, loc)
                        {
                                e->Run([&loc, this, &protoLocation] { loc = ParseLocation(protoLocation); });
                                if (!e->HasError())
#pragma omp critical
                                        m_geoGrid->AddLocation(move(loc));
                        }
                }
#pragma omp taskwait
        }
        e->Rethrow();
        AddCommutes(m_geoGrid);
        m_people.clear();
        m_commutes.clear();
        return m_geoGrid;
}

shared_ptr<Location> GeoGridProtoReader::ParseLocation(const proto::GeoGrid_Location& protoLocation)
{
        const auto  id         = protoLocation.id();
        const auto& name       = protoLocation.name();
        const auto  province   = protoLocation.province();
        const auto  population = protoLocation.population();
        const auto& coordinate = ParseCoordinate(protoLocation.coordinate());

        auto result = make_shared<Location>(id, province, population, coordinate, name);

        auto e = make_shared<ThreadException>();
#pragma omp parallel
#pragma omp single
        {
                for (int idx = 0; idx < protoLocation.contactcenters_size(); idx++) {
                        shared_ptr<ContactCenter>                    center;
                        const proto::GeoGrid_Location_ContactCenter& protoCenter = protoLocation.contactcenters(idx);
#pragma omp task firstprivate(protoCenter, center)
                        {
                                e->Run([&protoCenter, this, &center] { center = ParseContactCenter(protoCenter); });
                                if (!e->HasError())
#pragma omp critical
                                        result->AddContactCenter(center);
                        }
                }
#pragma omp taskwait
        }
        e->Rethrow();

        for (int idx = 0; idx < protoLocation.commutes_size(); idx++) {
                const proto::GeoGrid_Location_Commute& commute = protoLocation.commutes(idx);
#pragma omp critical
                m_commutes.emplace_back(make_tuple(id, commute.to(), commute.proportion()));
        }

        return result;
}

Coordinate GeoGridProtoReader::ParseCoordinate(const proto::GeoGrid_Location_Coordinate& protoCoordinate)
{
        return {protoCoordinate.longitude(), protoCoordinate.latitude()};
}

shared_ptr<ContactCenter> GeoGridProtoReader::ParseContactCenter(
    const proto::GeoGrid_Location_ContactCenter& protoContactCenter)
{
        const auto type = protoContactCenter.type();
        const auto id   = protoContactCenter.id();

        shared_ptr<ContactCenter> result;
        stride::ContactType::Id   typeId;
        switch (type) {
        case proto::GeoGrid_Location_ContactCenter_Type_K12School:
                result = make_shared<K12School>(id);
                typeId = stride::ContactType::Id::K12School;
                break;
        case proto::GeoGrid_Location_ContactCenter_Type_PrimaryCommunity:
                result = make_shared<PrimaryCommunity>(id);
                typeId = stride::ContactType::Id::PrimaryCommunity;
                break;
        case proto::GeoGrid_Location_ContactCenter_Type_SecondaryCommunity:
                result = make_shared<SecondaryCommunity>(id);
                typeId = stride::ContactType::Id::SecondaryCommunity;
                break;
        case proto::GeoGrid_Location_ContactCenter_Type_College:
                result = make_shared<College>(id);
                typeId = stride::ContactType::Id::College;
                break;
        case proto::GeoGrid_Location_ContactCenter_Type_Household:
                result = make_shared<Household>(id);
                typeId = stride::ContactType::Id::Household;
                break;
        case proto::GeoGrid_Location_ContactCenter_Type_Workplace:
                result = make_shared<Workplace>(id);
                typeId = stride::ContactType::Id::Workplace;
                break;
                break;
        default: throw runtime_error("No such ContactCenter type");
        }

        auto e = make_shared<ThreadException>();
#pragma omp parallel
#pragma omp single
        {
                for (int idx = 0; idx < protoContactCenter.pools_size(); idx++) {
                        const proto::GeoGrid_Location_ContactCenter_ContactPool& protoContactPool =
                            protoContactCenter.pools(idx);
#pragma omp task firstprivate(protoContactPool, typeId)
                        {
                                stride::ContactPool* pool = nullptr;
                                e->Run([&protoContactPool, &pool, this, &typeId] {
                                        pool = ParseContactPool(protoContactPool, typeId);
                                });
                                if (!e->HasError()) {
#pragma omp critical
                                        result->RegisterPool(pool);
                                }
                        }
                }
#pragma omp taskwait
        }
        e->Rethrow();

        return result;
}

stride::ContactPool* GeoGridProtoReader::ParseContactPool(
    const proto::GeoGrid_Location_ContactCenter_ContactPool& protoContactPool, stride::ContactType::Id type)
{
        // Don't use the id of the ContactPool but the let the Population create an id
        stride::ContactPool* result;

#pragma omp critical
        result = m_geoGrid->CreateContactPool(type);

        for (int idx = 0; idx < protoContactPool.people_size(); idx++) {
                const auto person_id = static_cast<unsigned int>(protoContactPool.people(idx));
                const auto person    = m_people.at(person_id);

#pragma omp critical
                {
                        result->AddMember(person);
                        // Update original pool id with new pool id used in the population
                        person->SetPoolId(type, static_cast<unsigned int>(result->GetId()));
                }
        }

        return result;
}

stride::Person* GeoGridProtoReader::ParsePerson(const proto::GeoGrid_Person& protoPerson)
{
        return m_geoGrid->CreatePerson(protoPerson.id(), protoPerson.age(), 0, 0, 0, 0, 0, 0);
}

} // namespace gengeopop
