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

#include "EpiHDF5Writer.h"

#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"
#include "util/Exception.h"
#include "util/HDF5.h"
#include <iostream>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

EpiHDF5Writer::EpiHDF5Writer(const string& fileName) : EpiFileWriter(fileName) {}

void EpiHDF5Writer::Write(std::vector<geopop::EnhancedCoordinate> locations)
{
        H5::Exception::dontPrint();

        H5File file;
        try {
                file = H5File(GetFileName(), H5F_ACC_TRUNC);
        } catch (FileIException& error) {
                throw util::Exception(error.getDetailMsg());
        }

        Group        locationshdf5 = file.createGroup("/locations");
        unsigned int count         = 0;
        const string name          = "location";
        for (const auto& location : locations) {
                ++count;
                string location_name = name + to_string(count);
                WriteLocation(location, locationshdf5, location_name);
        }
        WriteAttribute(count, "size", locationshdf5);
        file.close();
}

void EpiHDF5Writer::WriteCoordinate(const Coordinate& coordinate, Group& location)
{
        hsize_t   dims[2]    = {1, 2};
        double    data[1][2] = {{boost::geometry::get<0>(coordinate), boost::geometry::get<1>(coordinate)}};
        DataSpace dataspace(2, dims);
        Attribute attribute = location.createAttribute("coordinates", GetPredType<double>(), dataspace);
        attribute.write(GetPredType<double>(), data);
}

void EpiHDF5Writer::WriteLocation(const EnhancedCoordinate& location, Group& locations, const string& location_name)
{
        Group loc(locations.createGroup(location_name));
        WriteAttribute(location.getData<visualization::Location>()->id, "id", loc);
        WriteAttribute(location.getData<visualization::Location>()->name, "name", loc);
        WriteAttribute(location.getData<visualization::Location>()->size, "population", loc);
        WriteCoordinate(location.GetCoordinate(), loc);

        const string age[]    = {"College",   "Daycare",          "Household",          "K12School",
                              "PreSchool", "PrimaryCommunity", "SecondaryCommunity", "Workplace"};
        const string status[] = {"immune",      "infected",    "infectious", "recovered",
                                 "susceptible", "symptomatic", "total"};

        auto&              infected = location.getData<visualization::Location>()->infected;
        const unsigned int days     = location.getData<visualization::Location>()->infected["College"]["immune"].size();
        WriteAttribute(days, "days", loc);
        for (unsigned int a = 0; a < 8; a++) {
                Group ageGroup(loc.createGroup(age[a]));
                for (unsigned int s = 0; s < 7; s++) {
                        Group healtGroup(ageGroup.createGroup(status[s]));
                        for (unsigned int i = 0; i < days; i++) {
                                WriteAttribute(infected[age[a]][status[s]][i], to_string(i), healtGroup);
                        }
                }
        }
}

void EpiHDF5Writer::WriteAttribute(const string& data, const string& name, H5Object& object)
{
        hsize_t   dims[1] = {1};
        DataSpace dataspace(1, dims);
        Attribute attribute = object.createAttribute(name, GetPredType(data), dataspace);
        attribute.write(GetPredType(data), data);
}

template <typename T>
void EpiHDF5Writer::WriteAttribute(const T& data, const string& name, H5Object& object)
{
        hsize_t   dims[1] = {1};
        DataSpace dataspace(1, dims);
        T         data_buffer[1] = {data};
        Attribute attribute      = object.createAttribute(name, GetPredType<T>(), dataspace);
        attribute.write(GetPredType<T>(), data_buffer);
}

} // namespace geopop
