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
#include "util/HDF5.h"

#include <iostream>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

const unsigned int RANK = 1;

GeoGridHDF5Writer::GeoGridHDF5Writer(string& fileName)
    : GeoGridFileWriter(fileName), m_persons_found(), strdatatype(H5::PredType::C_S1, 256), person_type(sizeof(PERSON)), commute_type(sizeof(COMMUTE)), pool_type(sizeof(POOL))
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

void GeoGridHDF5Writer::Write(GeoGrid& geoGrid)
{
        H5File file(GetFileName(), H5F_ACC_TRUNC);

        Group locations = file.createGroup("/locations");
        unsigned int count = 0;
        const string name = "location";
        for (const auto& location : geoGrid) {
                count++;
                string location_name = name + to_string(count);
                WriteLocation(*location, locations, location_name);
                count++;
        }
        hsize_t attr_dims[1]      = {1};
        unsigned int attr_data[1] = {count};
        DataSpace   attr_dataspace(RANK, attr_dims);
        Attribute   attr_locations = locations.createAttribute("size", PredType::NATIVE_UINT, attr_dataspace);
        attr_locations.write(PredType::NATIVE_UINT, attr_data);

        hsize_t     dimsf[1] = {0};
        hsize_t     maxdimsf[2] = {H5S_UNLIMITED};
        DataSpace dataspace(RANK, dimsf, maxdimsf);
        DataSet persons = file.createDataSet("/persons", person_type, dataspace);
        for (const auto& person : m_persons_found) {
                WritePerson(person, persons);
        }
        unsigned long int attr_person_data[1] = {m_persons_found.size()};
        Attribute attr_person = persons.createAttribute("size", PredType::NATIVE_ULONG, attr_dataspace);
        attr_person.write(PredType::NATIVE_ULONG, attr_person_data);

        m_persons_found.clear();
        file.close();
}

void GeoGridHDF5Writer::WriteContactPool(stride::ContactPool* contactPool, Group& contactPools, const string& name)
{
        //contactPool_root.put("id", contactPool->GetId());
        hsize_t     dimsf[1]    = {0};
        hsize_t     maxdimsf[1] = {H5S_UNLIMITED};
        hsize_t attr_dims[1]    = {1};
        DataSpace dataspace(1, dimsf, maxdimsf);
        DataSpace attr_dataspace(RANK, attr_dims);
        DataSet pool = contactPools.createDataSet(name, PredType::NATIVE_UINT, dataspace);
        for (auto person : *contactPool) {
                m_persons_found.insert(person);
                //person_root.put("", person->GetId());
                //people.push_back(make_pair("", person_root));
                WritePeople(person, pool);
        }
        unsigned int attr_id_data[1]      = {contactPool->GetId()};
        unsigned long int attr_size_data[1]    = {contactPool->size()};
        string attr_type_data[1]    = {ToString(contactPool->GetType())};
        Attribute attr_id     = contactPools.createAttribute("id", PredType::NATIVE_UINT, dataspace);
        Attribute attr_size   = contactPools.createAttribute("size", PredType::NATIVE_ULONG, dataspace);
        Attribute attr_type   = contactPools.createAttribute("type", strdatatype, dataspace);
        attr_id.write(PredType::NATIVE_UINT, attr_id_data);
        attr_size.write(PredType::NATIVE_ULONG, attr_size_data);
        attr_type.write(PredType::NATIVE_UINT, attr_type_data);
}

void GeoGridHDF5Writer::WriteCoordinate(const Coordinate& coordinate)
{

}

void GeoGridHDF5Writer::WriteLocation(const Location& location, Group& locations, const string& location_name)
{
        Group loc(locations.createGroup(location_name));

        hsize_t attr_dims[1]                = {1};
        hsize_t attr_coord_dims[2]          = {1,2};
        unsigned int attr_id_data[1]        = {location.GetID()};
        string attr_name_data[1]            = {location.GetName()};
        unsigned int attr_province_data[1]  = {location.GetProvince()};
        unsigned int attr_popcount_data[1]  = {location.GetPopCount()};
        double attr_coord_data[1][2]        = {
                                                {
                                                  boost::geometry::get<0>(location.GetCoordinate()),
                                                  boost::geometry::get<1>(location.GetCoordinate())
                                                }
                                              };
        DataSpace attr_dataspace(RANK, attr_dims);
        DataSpace attr_coord_dataspace(RANK+1, attr_coord_dims);
        Attribute attr_id       = locations.createAttribute("id", PredType::NATIVE_UINT, attr_dataspace);
        Attribute attr_name     = locations.createAttribute("name", strdatatype, attr_dataspace);
        Attribute attr_province = locations.createAttribute("province", PredType::NATIVE_UINT, attr_dataspace);
        Attribute attr_popcount = locations.createAttribute("population", PredType::NATIVE_UINT, attr_dataspace);
        Attribute attr_coord    = locations.createAttribute("coordinate", PredType::NATIVE_DOUBLE, attr_coord_dataspace);
        attr_id.write(PredType::NATIVE_UINT, attr_id_data);
        attr_name.write(strdatatype, attr_name_data);
        attr_province.write(PredType::NATIVE_UINT, attr_province_data);
        attr_popcount.write(PredType::NATIVE_UINT, attr_popcount_data);
        attr_coord.write(PredType::NATIVE_FLOAT, attr_coord_data);

        auto commutes = location.CRefOutgoingCommutes();
        if (!commutes.empty()) {
                hsize_t     dimsf[1] = {0};
                hsize_t     maxdimsf[1] = {H5S_UNLIMITED};

                DataSpace dataspace(RANK, dimsf, maxdimsf);
                DataSet commutes_dataset = loc.createDataSet("commutes", PredType::NATIVE_UINT, dataspace);
                for (auto commute_pair : commutes) {
                        WriteCommute(commute_pair, commutes_dataset);
                }
                unsigned long int attr_size_data[1] = {commutes.size()};
                Attribute attr_size = commutes_dataset.createAttribute("size", PredType::NATIVE_ULONG, attr_dataspace);
                attr_size.write(PredType::NATIVE_ULONG, attr_size_data);
        }

        Group contactPools(loc.createGroup("contactPools"));
        unsigned int count = 0;
        const string name = "pool";
        for (Id typ : IdList) {
                for (const auto& c : location.CRefPools(typ)) {
                        count++;
                        string pool_name = name + to_string(count);
                        WriteContactPool(c, contactPools, name);
                }
        }
        unsigned int attr_size_data[1] = {count};
        Attribute attr_size = contactPools.createAttribute("size", PredType::NATIVE_UINT, attr_dataspace);
        attr_size.write(PredType::NATIVE_UINT, attr_size_data);
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
