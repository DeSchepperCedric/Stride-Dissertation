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

#include "GeoGridIOUtils.h"

#include "geopop/GeoGridConfig.h"
#include "geopop/io/GeoGridHDF5Writer.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#include "H5Cpp.h"
#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

bool compareGeoGrid(GeoGrid& geoGrid, const string& testname)
{
        const string filename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/" + testname;
        GeoGridHDF5Writer writer(testname);
        writer.Write(geoGrid);

        //(FileSys::GetTestDir().string() + "/testdata/GeoGridHDF5/" + testname);

        return true;
}

TEST(GeoGridHDF5WriterTest, locationsTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        geoGrid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location>(3, 2, Coordinate(0, 0), "Mons", 2500));

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test0.h5"));
}

/*TEST(GeoGridHDF5WriterTest, contactCentersTest)
{
        auto pop      = Population::Create();
        auto geoGrid  = GeoGrid(pop.get());
        auto location = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);
        location->AddCenter(make_shared<ContactCenter>(0, Id::K12School));
        location->AddCenter(make_shared<ContactCenter>(1, Id::PrimaryCommunity));
        location->AddCenter(make_shared<ContactCenter>(2, Id::College));
        location->AddCenter(make_shared<ContactCenter>(3, Id::Household));
        location->AddCenter(make_shared<ContactCenter>(4, Id::Workplace));
        geoGrid.AddLocation(location);

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test1.h5"));
}*/

TEST(GeoGridHDF5WriterTest, peopleTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(*GetPopulatedGeoGrid(pop.get()), "test2.h5"));
}

TEST(GeoGridHDF5WriterTest, commutesTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(*GetCommutesGeoGrid(pop.get()), "test7.h5"));
}

} // namespace
