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

#include "MajorCitiesCSVReader.h"

#include "geopop/GeoGrid.h"
#include "geopop/Location.h"
#include "util/CSV.h"
#include "util/Exception.h"

#include <map>

namespace geopop {

using namespace std;
using namespace stride::util;

MajorCitiesCSVReader::MajorCitiesCSVReader(unique_ptr<istream> inputStream) : MajorCitiesReader(move(inputStream)) {}

void MajorCitiesCSVReader::FillGeoGrid(GeoGrid& geoGrid) const
{
        CSV reader(*(m_inputStream.get()));

        vector<unsigned int> header;

        for (const string& label : reader.GetLabels()) {
                //do nothing
        }

        for (const CSVRow& row : reader) {
                //do stuff
                const auto CityId = row.GetValue<unsigned int>(0);
                geoGrid.GetById(CityId)->SetMajor(true);
        }
}

} // namespace geopop
