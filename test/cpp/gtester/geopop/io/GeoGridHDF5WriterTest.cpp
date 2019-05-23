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

template <typename InputIterator1, typename InputIterator2>
bool range_equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
{
        while (first1 != last1 && first2 != last2) {
                if (*first1 != *first2)
                        return false;
                ++first1;
                ++first2;
        }
        return (first1 == last1) && (first2 == last2);
}

bool compareGeoGrid(GeoGrid& geoGrid, const string& testname)
{
        const string      filename        = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/test.h5";
        const string      comparefilename = FileSys::GetTestsDir().string() + "/testdata/GeoGridHDF5/" + testname;
        GeoGridHDF5Writer writer(filename);
        writer.Write(geoGrid);

        std::ifstream file1(filename, std::ifstream::binary | std::ifstream::ate);
        std::ifstream file2(comparefilename, std::ifstream::binary | std::ifstream::ate);

        std::istreambuf_iterator<char> begin1(file1);
        std::istreambuf_iterator<char> begin2(file2);

        std::istreambuf_iterator<char> end;

        return range_equal(begin1, end, begin2, end);
}

TEST(GeoGridHDF5WriterTest, locationsWrittenCorrectlyTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());

        auto loc1 = make_shared<Location>(1, 4, "Bavikhove", 2500);
        auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0.0, 0.0));
        auto loc2 = make_shared<Location>(2, 3, "Gent", 5000);
        auto coor2 = make_shared<EnhancedCoordinate>(loc2.get(), Coordinate(0.0, 0.0));
        auto loc3 = make_shared<Location>(3, 2, "Mons", 2500);
        auto coor3 = make_shared<EnhancedCoordinate>(loc3.get(), Coordinate(0.0, 0.0));

        geoGrid.addLocation(loc3, coor1);
        geoGrid.addLocation(loc3, coor2);
        geoGrid.addLocation(loc3, coor3);

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test0.h5"));
}

TEST(GeoGridHDF5WriterTest, contactPoolsWrittenCorrectlyTest)
{
        const auto pop     = Population::Create();
        auto&      geoGrid = pop->RefGeoGrid();
        const auto loc     = make_shared<Location>(1, 4, "Bavikhove", 2500);
        auto coor          = make_shared<EnhancedCoordinate>(loc.get(), Coordinate(0.0,0.0));

        loc->RefPools(Id::K12School).emplace_back(pop->RefPoolSys().CreateContactPool(Id::K12School));
        loc->RefPools(Id::PrimaryCommunity).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity));
        loc->RefPools(Id::College).emplace_back(pop->RefPoolSys().CreateContactPool(Id::College));
        loc->RefPools(Id::Household).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Household));
        loc->RefPools(Id::Workplace).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Workplace));
        loc->RefPools(Id::Daycare).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Daycare));
        loc->RefPools(Id::PreSchool).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PreSchool));

        geoGrid.addLocation(loc, coor);

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test1.h5"));
}

TEST(GeoGridHDF5WriterTest, peopleWrittenCorrectlyTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(*GetPopulatedGeoGrid(pop.get()), "test2.h5"));
}

TEST(GeoGridHDF5WriterTest, commutesWrittenCorrectlyTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(*GetCommutesGeoGrid(pop.get()), "test7.h5"));
}

} // namespace
