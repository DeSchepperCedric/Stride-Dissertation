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

TEST(GeoGridHDF5WriterTest, locationsTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        geoGrid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location>(3, 2, Coordinate(0, 0), "Mons", 2500));

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test0.h5"));
}

TEST(GeoGridHDF5WriterTest, contactPoolsTest)
{
        const auto pop     = Population::Create();
        auto&      geoGrid = pop->RefGeoGrid();
        const auto loc     = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        loc->RefPools(Id::K12School).emplace_back(pop->RefPoolSys().CreateContactPool(Id::K12School));
        loc->RefPools(Id::PrimaryCommunity).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity));
        loc->RefPools(Id::College).emplace_back(pop->RefPoolSys().CreateContactPool(Id::College));
        loc->RefPools(Id::Household).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Household));
        loc->RefPools(Id::Workplace).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Workplace));
        loc->RefPools(Id::Daycare).emplace_back(pop->RefPoolSys().CreateContactPool(Id::Daycare));
        loc->RefPools(Id::PreSchool).emplace_back(pop->RefPoolSys().CreateContactPool(Id::PreSchool));

        geoGrid.AddLocation(loc);

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test1.h5"));
}

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
