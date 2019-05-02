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

#include "GeoGridHDF5Writer.h"

#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"

#include <iostream>
#include <sstream>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace H5;

const unsigned int RANK = 1;

GeoGridHDF5Writer::GeoGridHDF5Writer() : m_persons_found(), person_type(sizeof(PERSON)), commute_type(sizeof(COMMUTE)), pool_type(sizeof(POOL))
{
        person_type.insertMember("Id", HOFFSET(PERSON, id), PredType::NATIVE_UINT);
        person_type.insertMember("Age", HOFFSET(PERSON, age), PredType::NATIVE_FLOAT);
        person_type.insertMember("K12School", HOFFSET(PERSON, k12school), PredType::NATIVE_UINT);
        person_type.insertMember("College", HOFFSET(PERSON, college), PredType::NATIVE_UINT);
        person_type.insertMember("Household", HOFFSET(PERSON, household), PredType::NATIVE_UINT);
        person_type.insertMember("Workplace", HOFFSET(PERSON, workplace), PredType::NATIVE_UINT);
        person_type.insertMember("PrimaryCommunity", HOFFSET(PERSON, primarycommunity), PredType::NATIVE_UINT);
        person_type.insertMember("SecondaryCommunity", HOFFSET(PERSON, secondarycommunity), PredType::NATIVE_UINT);
        person_type.insertMember("Daycare", HOFFSET(PERSON, daycare), PredType::NATIVE_UINT);
        person_type.insertMember("PreSchool", HOFFSET(PERSON, preschool), PredType::NATIVE_UINT);

        commute_type.insertMember("To", HOFFSET(COMMUTE, to), PredType::NATIVE_UINT);
        commute_type.insertMember("Proportion", HOFFSET(COMMUTE, proportion), PredType::NATIVE_DOUBLE);

        pool_type.insertMember("People", HOFFSET(POOL, people), PredType::NATIVE_UINT);
}

void GeoGridHDF5Writer::Write(GeoGrid& geoGrid, std::ostream& stream)
{
        ostringstream ss;
        ss << stream.rdbuf();
        H5File file(ss.str(), H5F_ACC_TRUNC);

        Group locations = file.createGroup("/locations");
        int count = 1;
        const string name = "location";
        for (const auto& location : geoGrid) {
                string location_name = name + to_string(count);
                WriteLocation(*location, locations, location_name);
                count++;
        }
        //TODO attribute size

        hsize_t     dimsf[1] = {0};
        hsize_t     maxdimsf[2] = {H5S_UNLIMITED};
        DataSpace dataspace(RANK, dimsf, maxdimsf);
        DataSet persons = file.createDataSet("/persons", person_type, dataspace);
        for (const auto& person : m_persons_found) {
                WritePerson(person, persons);
        }
        /*hsize_t     dims[1];
        DataSpace size_dataspace(1, dims)
        Attribute persons_size = persons.createAttribute("size", PredType::NATIVE_INT, size_dataspace);
        persons_size.write(PredType::NATIVE_INT, m_persons_found.size());
        */
        m_persons_found.clear();
        file.close();
}

void GeoGridHDF5Writer::WriteContactPool(stride::ContactPool* contactPool, Group& contactPools, const string& name)
{
        //contactPool_root.put("id", contactPool->GetId());
        hsize_t     dimsf[1] = {0};
        hsize_t     maxdimsf[1] = {H5S_UNLIMITED};
        DataSpace dataspace(1, dimsf, maxdimsf);
        DataSet pool = contactPools.createDataSet(name, PredType::NATIVE_UINT, dataspace);
        for (auto person : *contactPool) {
                m_persons_found.insert(person);
                //person_root.put("", person->GetId());
                //people.push_back(make_pair("", person_root));
                WritePeople(person, pool);
        }
}

void GeoGridHDF5Writer::WriteCoordinate(const Coordinate& coordinate)
{

}

void GeoGridHDF5Writer::WriteLocation(const Location& location, Group& locations, const string& location_name)
{
        Group loc(locations.createGroup(location_name));

        /*
                    location.GetID(),
                    location.GetName(),
                    location.GetProvince(),
                    location.GetPopCount(),
                    location.GetCoordinate()
        */

        auto commutes = location.CRefOutgoingCommutes();
        if (!commutes.empty()) {
                hsize_t     dimsf[1] = {0};
                hsize_t     maxdimsf[1] = {H5S_UNLIMITED};

                DataSpace dataspace(RANK, dimsf, maxdimsf);
                DataSet commutes_dataset = loc.createDataSet("commutes", PredType::NATIVE_UINT, dataspace);
                for (auto commute_pair : commutes) {
                        WriteCommute(commute_pair, commutes_dataset);
                }
                //TODO attributes
        }

        Group contactPools(loc.createGroup("contactPools"));

        int count = 1;
        const string name = "pool";
        for (Id typ : IdList) {
                for (const auto& c : location.CRefPools(typ)) {
                        string pool_name = name + to_string(count);
                        WriteContactPool(c, contactPools, name);
                        count++;
                }
        }
}

void GeoGridHDF5Writer::WritePeople(stride::Person* person, DataSet& pool)
{
        hsize_t dimext[1] = {1};
        pool.extend(pool.getStorageSize() + dimext);

        POOL pool_add;
        pool_add.people = person->GetId();

        POOL data[1] = {pool_add};

        DataSpace memspace(RANK, dimext, NULL);
        pool.write(data, pool_type, memspace);
}

void GeoGridHDF5Writer::WriteCommute(pair<Location*, double> commute_pair,  DataSet& locations)
{
        hsize_t dimext[1] = {1};
        locations.extend(locations.getStorageSize() + dimext);

        COMMUTE commute_add;
        commute_add.to          = commute_pair.first->GetID();
        commute_add.proportion  = commute_pair.second;

        COMMUTE data[1] = {commute_add};

        DataSpace memspace(RANK, dimext, NULL);
        locations.write(data, commute_type, memspace);

}

void GeoGridHDF5Writer::WritePerson(stride::Person* person, DataSet& persons)
{
        hsize_t dimext[1] = {1};
        persons.extend(persons.getStorageSize() + dimext);

        PERSON person_add;
        person_add.id                 = person->GetId();
        person_add.age                = person->GetAge();
        person_add.k12school          = person->GetPoolId(Id::K12School);
        person_add.college            = person->GetPoolId(Id::College);
        person_add.household          = person->GetPoolId(Id::Household);
        person_add.workplace          = person->GetPoolId(Id::Workplace);
        person_add.primarycommunity   = person->GetPoolId(Id::PrimaryCommunity);
        person_add.secondarycommunity = person->GetPoolId(Id::SecondaryCommunity);
        person_add.daycare            = person->GetPoolId(Id::Daycare);
        person_add.preschool          = person->GetPoolId(Id::PreSchool);

        PERSON data[1] = {person_add};

        DataSpace memspace(RANK, dimext, NULL);
        persons.write(data, person_type, memspace);
}

/*
template<typename T, typename type>
void GeoGridHDF5Writer::WriteAttribute(T& element, type data, const string name)
{
        hsize_t     dims[1];
        DataSpace dataspace(1, dims)
        Attribute attribute = element->createAttribute(name, PredType::NATIVE_INT, dataspace);
        element->write(PredType::NATIVE_INT, );
}
*/

} // namespace geopop
