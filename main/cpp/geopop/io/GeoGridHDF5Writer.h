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
#include "geopop/EnhancedCoordinate.h"

#include "H5Cpp.h"
#include <set>
#include <string>

namespace stride {
class ContactPool;
class Person;
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
        void WriteCoordinate(const Coordinate& coordinate);

        /// Create a HDF5 Group containing all info needed to reconstruct a Location.
        void WriteLocation(const geopop::EnhancedCoordinate& location, H5::Group& locations, const std::string& location_name);

        /// Write to a HDF5 Dataset containing all info needed to reconstruct a pool.
        void WritePeople(stride::Person* person, H5::DataSet& pool);

        /// Write to a HDF5 Dataset containing all info needed to reconstruct a commute.
        void WriteCommute(std::pair<Location*, double> commute_pair, H5::DataSet& locations);

        /// Write to a HDF5 Dataset containing all info needed to reconstruct a Person.
        void WritePerson(stride::Person* person, H5::DataSet& persons);

private:
        std::set<stride::Person*> m_persons_found; ///< The persons found when looping over the ContactPools.

        const H5::StrType strdatatype;
        H5::CompType      person_type;
        H5::CompType      commute_type;
        H5::CompType      pool_type;
};

} // namespace geopop
