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

const unsigned int RANK       = 2;
const unsigned int DIM2       = 1;
const unsigned int PRSNS_DIM1 = 10;
const unsigned int CMMT_DIM1  = 2;
const unsigned int PPL_DIM1   = 1;

GeoGridHDF5Writer::GeoGridHDF5Writer() : m_persons_found() {}

void GeoGridHDF5Writer::Write(GeoGrid& geoGrid, std::ostream& stream)
{
        ostringstream ss;
        ss << stream.rdbuf();
        H5File file(ss.str(), H5F_ACC_TRUNC);

        Group locations = file.createGroup("/Locations");
        int count = 1;
        const string name = "location";
        for (const auto& location : geoGrid) {
                string location_name = name + to_string(count);
                WriteLocation(*location, locations, location_name);
                count++;
        }
        //TODO attribute size

        hsize_t     dimsf[2];
        hsize_t     maxdimsf[2] = {H5S_UNLIMITED, H5S_UNLIMITED};
        dimsf[0] = DIM2;
        dimsf[1] = PRSNS_DIM1;
        DataSpace dataspace(RANK, dimsf, maxdimsf);
        DataSet persons = file.createDataSet("/persons", PredType::NATIVE_UINT, dataspace);
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
        hsize_t     dimsf[1];
        hsize_t     maxdimsf[1] = {H5S_UNLIMITED};
        dimsf[0] = PPL_DIM1;
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
                hsize_t     dimsf[2];
                hsize_t     maxdimsf[2] = {H5S_UNLIMITED, H5S_UNLIMITED};
                dimsf[0] = DIM2;
                dimsf[1] = CMMT_DIM1;
                DataSpace dataspace(RANK, dimsf, maxdimsf);
                DataSet commutes_dataset = loc.createDataSet("commutes", PredType::NATIVE_UINT, dataspace);
                for (auto commute_pair : commutes) {
                        //WriteCommute(DataSet& commutes_dataset, auto commute_pair);
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
        unsigned int data[PPL_DIM1] = {person->GetId()};
        pool.write(data, PredType::NATIVE_UINT);
}

void GeoGridHDF5Writer::WriteCommute(DataSet& locations)
{
        //locations->write(,)
}

void GeoGridHDF5Writer::WritePerson(stride::Person* person, DataSet& persons)
{
        unsigned int data[DIM2][PRSNS_DIM1] =
                {
                  {
                    person->GetId(),
                    person->GetAge(),
                    person->GetPoolId(Id::K12School),
                    person->GetPoolId(Id::College),
                    person->GetPoolId(Id::Household),
                    person->GetPoolId(Id::Workplace),
                    person->GetPoolId(Id::PrimaryCommunity),
                    person->GetPoolId(Id::SecondaryCommunity),
                    person->GetPoolId(Id::Daycare),
                    person->GetPoolId(Id::PreSchool)
                  }
                };

        persons.write(data, PredType::NATIVE_UINT);
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
