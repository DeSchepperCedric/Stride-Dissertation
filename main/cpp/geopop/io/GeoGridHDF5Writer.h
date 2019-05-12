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

#pragma once

#include "GeoGridFileWriter.h"
#include "geopop/Location.h"

#include "H5Cpp.h"
#include <string>
#include <set>

namespace stride {
class ContactPool;
class Person;

namespace util {
struct PersonType;
struct CommuteType;
struct PoolType;
} // namespace util

} // namespace stride

namespace geopop {

class ContactCenter;

/**
 * Writes a GeoGrid to a HDF5 file.
 */
class GeoGridHDF5Writer : public GeoGridFileWriter
{
public:
        /// Construct the GeoGridHDF5Writer.
        GeoGridHDF5Writer(const std::string& fileName);

        /// Write the provided GeoGrid to the proved ostream in HDF5 format.
        void Write(GeoGrid& geoGrid) override;

private:
        /// Create a HDF5 containing all info needed to reconstruct a ContactPool.
        void WriteContactPool(stride::ContactPool* contactPool, H5::Group& contactPools, const std::string& name);

        /// Create a HDF5 containing all info needed to reconstruct a Coordinate.
        void WriteCoordinate(const Coordinate& coordinate, H5::Group& location);

        /// Create a HDF5 Group containing all info needed to reconstruct a Location.
        void WriteLocation(const Location& location, H5::Group& locations, const std::string& location_name);

        /// Write to a HDF5 Dataset containing all info needed to reconstruct a Person.
        stride::util::PersonType WritePerson(stride::Person* person);//, H5::DataSet& persons);

        /// Write to a HDF5 Dataset containing all info needed to reconstruct a commute.
        stride::util::CommuteType WriteCommute(std::pair<Location*, double> commute_pair);

        /// Write to a HDF5 Dataset containing all info needed to reconstruct a pool.
        stride::util::PoolType WritePool(stride::Person* person);

        /// Write an attribute of size 1 for data to a group or dataset
        template<typename T>
        void WriteAttribute(const T& data, const std::string& name, H5::H5Object& object);

private:
        std::set<stride::Person*> m_persons_found; ///< The persons found when looping over the ContactPools.
};

template<>
void GeoGridHDF5Writer::WriteAttribute(const std::string& data, const std::string& name, H5::H5Object& object);

} // namespace geopop
