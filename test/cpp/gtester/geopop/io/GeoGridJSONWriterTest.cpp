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

#include "geopop/ContactCenter.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/io/GeoGridJSONWriter.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using boost::property_tree::ptree;

namespace {

void sortContactCenters(nlohmann::json& json)
{
        auto& contactCenters       = json.at("contactCenters");
        auto  compareContactCenter = [](const nlohmann::json& a, const nlohmann::json& b) {
                return a.at("type") < b.at("type");
        };
        std::sort(contactCenters.begin(), contactCenters.end(), compareContactCenter);
}
/*
void sortTree(ptree& tree)
{
        auto compareLocation = [](const pair<string, ptree>& a, const pair<string, ptree>& b) {
                return a.second.get<string>("id") < b.second.get<string>("id");
        };
        auto& locations = tree.get_child("locations");
        locations.sort<decltype(compareLocation)>(compareLocation);

        for (auto it = locations.begin(); it != locations.end(); it++) {
                sortContactCenters(it->second.get_child(""));
        }
}
*/
void sortJSON(nlohmann::json& json)
{
    auto compareLocation = [](const nlohmann::json& a, const nlohmann::json& b) {
        return a.at("id") < b.at("id");
    };
    auto& locations = json.at("locations");
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
        std::ifstream inputStream(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/writerJSON/" + testname);

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
        auto pop      = Population::Create();
        auto geoGrid  = GeoGrid(pop.get());
        auto location = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        location->AddCenter(make_shared<ContactCenter>(0, Id::K12School));
        location->AddCenter(make_shared<ContactCenter>(1, Id::PrimaryCommunity));
        location->AddCenter(make_shared<ContactCenter>(2, Id::College));
        location->AddCenter(make_shared<ContactCenter>(3, Id::Household));
        location->AddCenter(make_shared<ContactCenter>(4, Id::Workplace));
        location->AddCenter(make_shared<ContactCenter>(5, Id::Daycare));
        location->AddCenter(make_shared<ContactCenter>(6, Id::PreSchool));
        geoGrid.AddLocation(location);

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
