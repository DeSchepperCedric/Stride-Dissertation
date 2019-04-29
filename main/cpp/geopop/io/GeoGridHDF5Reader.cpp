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

#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <memory>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

GeoGridHDF5Reader::GeoGridhdf5Reader(unique_ptr<istream> inputStream, Population* pop)
    : GeoGridReader(move(inputStream), pop)
{
}

void GeoGridHDF5Reader::Read()
{
        try {
                H5File file(*m_inputStream, H5F_ACC_RDONLY);
        } catch (FileIException error) {
                throw Exception(error.getDetailMsg());
        } catch (runtime_error&) {
                throw Exception("Problem parsing HDF5 file, check whether empty or invalid HDF5.");
        }

        auto& geoGrid = m_population->RefGeoGrid();
        auto people = file.openDataSet("persons");

        AddCommutes(geoGrid);
        m_commutes.clear();
        m_people.clear();
}

Person* GeoGridHDF5Reader::ParsePerson(DataSet& person)
{
        const auto id   = person.
        const auto age  =
        const auto hhId =
        const auto ksId =
        const auto coId =
        const auto wpId =
        const auto pcId =
        const auto dcId =
        const auto prId =
}

} // namespace geopop
