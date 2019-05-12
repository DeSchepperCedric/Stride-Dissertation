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
#include "util/HDF5.h"
#include "pop/Person.h"
#include <iostream>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

const unsigned int RANK = 1;

GeoGridHDF5Writer::GeoGridHDF5Writer(const string& fileName)
    : GeoGridFileWriter(fileName), m_persons_found()
{
}

void GeoGridHDF5Writer::Write(GeoGrid& geoGrid)
{
        H5File file(GetFileName(), H5F_ACC_TRUNC);

        Group locations = file.createGroup("/locations");
        unsigned int count = 0;
        const string name = "location";
        for (const auto& location : geoGrid) {
                ++count;
                string location_name = name + to_string(count);
                WriteLocation(*location, locations, location_name);
        }
        WriteAttribute(count, "size", locations);

        hsize_t   dimsf[1] = {m_persons_found.size()};
        DataSpace dataspace(RANK, dimsf);
        DataSet persons = file.createDataSet("/persons", GetPersonType(), dataspace);
        vector<PersonType> persons_data;
        for (const auto& person : m_persons_found) {
                persons_data.push_back(WritePerson(person));
        }
        persons.write(persons_data.data(), GetPersonType());
        WriteAttribute(m_persons_found.size(), "size", persons);

        m_persons_found.clear();
        file.close();
}

void GeoGridHDF5Writer::WriteContactPool(stride::ContactPool* contactPool, Group& contactPools, const string& name)
{
        hsize_t   dimsf[1] = {contactPool->size()};
        DataSpace dataspace(RANK, dimsf);
        DataSet pool = contactPools.createDataSet(name, GetPoolType(), dataspace);
        vector<PoolType> pool_data;
        for (auto person : *contactPool) {
                m_persons_found.insert(person);
                pool_data.push_back(WritePool(person));
        }
        pool.write(pool_data.data(), GetPoolType());
        WriteAttribute(contactPool->GetId(), "id",   pool);
        WriteAttribute(contactPool->size(), "size", pool);
        WriteAttribute(ToString(contactPool->GetType()), "type", pool);
}

void GeoGridHDF5Writer::WriteCoordinate(const Coordinate& coordinate, Group& location)
{
        hsize_t dims[2]     = {1,2};
        double  data[1][2]  = {
                                  {
                                      boost::geometry::get<0>(coordinate),
                                      boost::geometry::get<1>(coordinate)
                                  }
                              };
        DataSpace dataspace(2, dims);
        Attribute attribute = location.createAttribute("coordinates", GetPredType(data[0][0]), dataspace);
        attribute.write(GetPredType(data[0][0]), data);

}

void GeoGridHDF5Writer::WriteLocation(const Location& location, Group& locations, const string& location_name)
{
        Group loc(locations.createGroup(location_name));
        WriteAttribute(location.GetID(), "id", loc);
        WriteAttribute(location.GetName(), "name", loc);
        WriteAttribute(location.GetProvince(), "province", loc);
        WriteAttribute(location.GetPopCount(), "population", loc);
        WriteCoordinate(location.GetCoordinate(), loc);

        auto commutes = location.CRefOutgoingCommutes();
        hsize_t     dimsf[1] = {commutes.size()};
        DataSpace dataspace(RANK, dimsf);
        DataSet commutes_dataset = loc.createDataSet("commutes", GetCommuteType(), dataspace);
        if (!commutes.empty()) {
                vector<CommuteType> commute_data;
                for (auto commute_pair : commutes) {
                        commute_data.push_back(WriteCommute(commute_pair));
                }
                commutes_dataset.write(commute_data.data(), GetCommuteType());
        }
        unsigned int commutes_size = commutes.size();
        WriteAttribute(commutes_size, "size", commutes_dataset);

        Group contactPools(loc.createGroup("contactPools"));
        unsigned int count = 0;
        const string name = "pool";
        for (Id typ : IdList) {
                for (const auto& c : location.CRefPools(typ)) {
                        ++count;
                        string pool_name = name + to_string(count);
                        WriteContactPool(c, contactPools, pool_name);
                }
        }
        WriteAttribute(count, "size", contactPools);
}

PersonType GeoGridHDF5Writer::WritePerson(stride::Person* person)
{
        PersonType person_add;
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

        return person_add;
}

CommuteType GeoGridHDF5Writer::WriteCommute(pair<Location*, double> commute_pair)
{
        CommuteType commute;
        commute.to          = commute_pair.first->GetID();
        commute.proportion  = commute_pair.second;

        return commute;
}

PoolType GeoGridHDF5Writer::WritePool(stride::Person* person)
{
        PoolType pool;
        pool.people = person->GetId();

        return pool;
}

template<typename T>
void GeoGridHDF5Writer::WriteAttribute(const T& data, const string& name, H5Object& object)
{
        hsize_t   dims[1] = {1};
        DataSpace dataspace(1, dims);
        T         data_buffer[1] = {data};
        Attribute attribute = object.createAttribute(name, GetPredType(data), dataspace);
        attribute.write(GetPredType(data), data_buffer);
}

template<>
void GeoGridHDF5Writer::WriteAttribute(const string& data, const string& name, H5Object& object)
{
        hsize_t   dims[1] = {1};
        DataSpace dataspace(1, dims);
        Attribute attribute = object.createAttribute(name, GetPredType(data), dataspace);
        attribute.write(GetPredType(data), data);
}

} // namespace geopop
