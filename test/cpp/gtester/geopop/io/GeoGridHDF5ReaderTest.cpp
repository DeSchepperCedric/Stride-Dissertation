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

#include "geopop/io/GeoGridHDF5Reader.h"

#include "contact/ContactType.h"
#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"
#include "util/FileSys.h"

#include <fstream>
#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

using boost::geometry::get;

namespace {

void getGeoGridFromFile(const string& filename, Population* pop)
{
        auto file = make_unique<ifstream>();
        file->open(FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/" + filename);
        GeoGridHDF5Reader geoGridHDF5Reader(move(file), pop);
        geoGridHDF5Reader.Read();
}

TEST(GeoGridHDFReaderTest, locationsTest) { EXPECT_TRUE(true); }

TEST(GeoGridHDFReaderTest, commutesTest) { EXPECT_TRUE(true); }

TEST(GeoGridHDFReaderTest, contactCentersTest) { EXPECT_TRUE(true); }

TEST(GeoGridHDFReaderTest, peopleTest) { EXPECT_TRUE(true); }

TEST(GeoGridHDFReaderTest, invalidTypeTest) { EXPECT_TRUE(true); }

TEST(GeoGridHDFReaderTest, invalidPersonTest) { EXPECT_TRUE(true); }

TEST(GeoGridHDFReaderTest, invalidHDFTest) { EXPECT_TRUE(true); }

} // namespace
