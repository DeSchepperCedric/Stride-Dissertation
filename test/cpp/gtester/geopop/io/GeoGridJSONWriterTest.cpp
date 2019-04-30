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

#include "GeoGridIOUtils.h"

//#include "geopop/ContactCenter.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/io/GeoGridJSONWriter.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
//using boost::property_tree::ptree;

namespace {

void sortContactCenters(nlohmann::json& json)
{
        auto& contactCenters       = json.at("contactPools");
        auto  compareContactCenter = [](const nlohmann::json& a, const nlohmann::json& b) {
                return a.at("class") < b.at("class");
        };
        std::sort(contactCenters.begin(), contactCenters.end(), compareContactCenter);
}

void sortJSON(nlohmann::json& json)
{
        auto compareLocation = [](const nlohmann::json& a, const nlohmann::json& b) { return a.at("id") < b.at("id"); };
        auto& locations      = json.at("locations");
        std::sort(locations.begin(), locations.end(), compareLocation);

        for (auto it = locations.begin(); it != locations.end(); it++) {
                sortContactCenters(*it);
        }
}

bool compareGeoGrid(GeoGrid& geoGrid, const string& testname)
{
        GeoGridJSONWriter writer;
        stringstream      ss;
        writer.Write(geoGrid, ss);
        nlohmann::json result;
        ss >> result;
        sortJSON(result);
        nlohmann::json expected;
        std::ifstream  inputStream(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/writerJSON/" + testname);

        inputStream >> expected;
        sortJSON(expected);

        return result == expected;
}

TEST(GeoGridJSONWriterTest, locationTest)
{
        auto pop     = Population::Create();
        auto geoGrid = GeoGrid(pop.get());
        geoGrid.AddLocation(make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500));
        geoGrid.AddLocation(make_shared<Location>(2, 3, Coordinate(0, 0), "Gent", 5000));
        geoGrid.AddLocation(make_shared<Location>(3, 2, Coordinate(0, 0), "Mons", 2500));

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test0.json"));
}
TEST(GeoGridJSONWriterTest, contactCentersTest)
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

        EXPECT_TRUE(compareGeoGrid(geoGrid, "test1.json"));
}

TEST(GeoGridJSONWriterTest, peopleTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(*GetPopulatedGeoGrid(pop.get()), "test2.json"));
}

TEST(GeoGridJSONWriterTest, commutesTest)
{
        auto pop = Population::Create();
        EXPECT_TRUE(compareGeoGrid(*GetCommutesGeoGrid(pop.get()), "test7.json"));
}

} // namespace
