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

#include "GeoGridFileReader.h"
#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "geopop/EnhancedCoordinate.h"
#include "geopop/Location.h"

#include "H5Cpp.h"
#include "util/HDF5.h"
#include <boost/lexical_cast.hpp>

namespace geopop {

class GeoGrid;

/**
 * An implementation of the GeoGridReader using HDF5.
 * This class is used to read a GeoGrid from a HDF5 file.
 */
class GeoGridHDF5Reader : public GeoGridFileReader
{
public:
        /// Construct the GeoGridHDF5Reader with the file name which contains the HDF5.
        GeoGridHDF5Reader(const std::string& inputFile, stride::Population* pop);

        /// No copy constructor.
        GeoGridHDF5Reader(const GeoGridHDF5Reader&) = delete;

        /// No copy assignement.
        GeoGridHDF5Reader operator=(const GeoGridHDF5Reader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        void Read() override;

private:
        /// Create a ContactCenter based on the information stored in the provided ...
        /// std::shared_ptr<ContactCenter> ParseContactCenter(H5::& contactCenter);

        /// Create a ContactCenter based on the information stored in the provided ...
        void ParseContactPool(H5::DataSet& contactPool, const std::shared_ptr<Location>& result);

        /// Create a Coordinate based on the information stored in the provided ...
        // Coordinate ParseCoordinate(H5::Attribute& coordinate);

        /// Create a Location based on the information stored in the provided ...
        std::pair<std::shared_ptr<Location>, std::shared_ptr<EnhancedCoordinate>> ParseLocation(H5::Group& location);

        /// Create a Person based on the information stored in the provided ...
        stride::Person* ParsePerson(stride::util::PersonType& person);

        template <typename T>
        T ReadAttribute(const std::string& name, H5::H5Object& object);
};

template <>
std::string GeoGridHDF5Reader::ReadAttribute(const std::string& name, H5::H5Object& object);

} // namespace geopop
