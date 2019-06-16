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

#include "geopop/io/GeoGridJSONReader.h"

#include "contact/ContactType.h"
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
        //        std::cout <<FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/" + filename << std::endl;
        file->open(FileSys::GetTestsDir().string() + "/testdata/GeoGridJSON/" + filename);
        GeoGridJSONReader geoGridJSONReader(move(file), pop);
        geoGridJSONReader.Read();
}

TEST(GeoGridJSONReaderTest, locationsParsedCorrectlyTest)
{
        auto pop = Population::Create();
        getGeoGridFromFile("test0.json", pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        map<unsigned int, Location*> locations;
        locations[geoGrid[0]->GetID()] = geoGrid[0].get();
        locations[geoGrid[1]->GetID()] = geoGrid[1].get();
        locations[geoGrid[2]->GetID()] = geoGrid[2].get();

        const auto location1 = locations[1];
        const auto location2 = locations[2];
        const auto location3 = locations[3];

        EXPECT_EQ(location1->GetID(), 1);
        EXPECT_EQ(location1->GetName(), "Bavikhove");
        EXPECT_EQ(location1->GetProvince(), 4);
        EXPECT_EQ(location1->GetPopCount(), 2500);

        EXPECT_EQ(location2->GetID(), 2);
        EXPECT_EQ(location2->GetName(), "Gent");
        EXPECT_EQ(location2->GetProvince(), 3);
        EXPECT_EQ(location2->GetPopCount(), 5000);

        EXPECT_EQ(location3->GetID(), 3);
        EXPECT_EQ(location3->GetName(), "Mons");
        EXPECT_EQ(location3->GetProvince(), 2);
        EXPECT_EQ(location3->GetPopCount(), 2500);
}

TEST(GeoGridJSONReaderTest, commutesParsedCorrectlyTest)
{
        auto pop = Population::Create();
        getGeoGridFromFile("test7.json", pop.get());
        auto& geoGrid = pop->RefGeoGrid();

        map<unsigned int, Location*> locations;

        locations[geoGrid[0]->GetID()] = geoGrid[0].get();
        locations[geoGrid[1]->GetID()] = geoGrid[1].get();
        locations[geoGrid[2]->GetID()] = geoGrid[2].get();

        auto location1 = locations[1];
        auto location2 = locations[2];
        auto location3 = locations[3];

        auto sortLoc = [](vector<pair<Location*, double>> loc) {
                sort(begin(loc), end(loc), [](const pair<Location*, double>& a, const pair<Location*, double>& b) {
                        return a.first->GetID() < b.first->GetID();
                });
                return loc;
        };

        {
                auto commuting_in  = sortLoc(location1->CRefIncomingCommutes());
                auto commuting_out = sortLoc(location1->CRefOutgoingCommutes());
                EXPECT_EQ(commuting_in.size(), 1);
                EXPECT_EQ(commuting_out.size(), 2);

                EXPECT_EQ(commuting_in[0].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.75);

                EXPECT_EQ(commuting_out[0].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_out[0].second, 0.50);
                EXPECT_EQ(commuting_out[1].first->GetID(), 3);
                EXPECT_DOUBLE_EQ(commuting_out[1].second, 0.25);
        }
        {
                auto commuting_in  = sortLoc(location2->CRefIncomingCommutes());
                auto commuting_out = sortLoc(location2->CRefOutgoingCommutes());
                EXPECT_EQ(commuting_out.size(), 2);
                EXPECT_EQ(commuting_in.size(), 1);

                EXPECT_EQ(commuting_in[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.50);

                EXPECT_EQ(commuting_out[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_out[0].second, 0.75);
                EXPECT_EQ(commuting_out[1].first->GetID(), 3);
                EXPECT_DOUBLE_EQ(commuting_out[1].second, 0.5);
        }
        {
                auto commuting_in  = sortLoc(location3->CRefIncomingCommutes());
                auto commuting_out = sortLoc(location3->CRefOutgoingCommutes());
                EXPECT_EQ(commuting_out.size(), 0);
                EXPECT_EQ(commuting_in.size(), 2);

                EXPECT_EQ(commuting_in[0].first->GetID(), 1);
                EXPECT_DOUBLE_EQ(commuting_in[0].second, 0.25);
                EXPECT_EQ(commuting_in[1].first->GetID(), 2);
                EXPECT_DOUBLE_EQ(commuting_in[1].second, 0.5);
        }
}

TEST(GeoGridJSONReaderTest, contactPoolsParsedCorrectlyTest)
{
        auto pop = Population::Create();
        getGeoGridFromFile("test1.json", pop.get());
        auto& geoGrid  = pop->RefGeoGrid();
        auto  location = geoGrid[0];

        vector<shared_ptr<ContactPool>> centers;
        for (Id typ : IdList) {
                for (const auto& p : location->CRefPools(typ)) {
                        centers.emplace_back(p);
                }
        }

        for (Id typ : IdList) {
                EXPECT_EQ(location->CRefPools(typ).size(), 0);
        }
}

void runPeopleTest(const string& filename)
{
        auto pop = Population::Create();
        getGeoGridFromFile(filename, pop.get());
        auto& geoGrid  = pop->RefGeoGrid();
        auto& grid     = *geoGrid.m_locationGrid.get();
        auto  location = grid[0];

        EXPECT_EQ(location->getData<Location>()->GetID(), 1);
        EXPECT_EQ(location->getData<Location>()->GetName(), "Bavikhove");
        EXPECT_EQ(location->getData<Location>()->GetProvince(), 4);
        EXPECT_EQ(location->getData<Location>()->GetPopCount(), 2500);
        EXPECT_EQ(get<0>(location->GetCoordinate()), 0);
        EXPECT_EQ(get<1>(location->GetCoordinate()), 0);

        vector<ContactPool*> centers;
        for (Id typ : IdList) {
                for (const auto& p : location->getData<Location>()->CRefPools(typ)) {
                        centers.emplace_back(p);
                }
        }

        for (const auto& center : centers) {
                auto person = (*center)[0];
                EXPECT_EQ(person->GetId(), 0);
                EXPECT_EQ(person->GetAge(), 18);
                EXPECT_EQ(person->GetPoolId(Id::K12School), 1);
                EXPECT_EQ(person->GetPoolId(Id::Household), 1);
                EXPECT_EQ(person->GetPoolId(Id::College), 1);
                EXPECT_EQ(person->GetPoolId(Id::Workplace), 1);
                EXPECT_EQ(person->GetPoolId(Id::PrimaryCommunity), 1);
                EXPECT_EQ(person->GetPoolId(Id::SecondaryCommunity), 1);
                EXPECT_EQ(person->GetPoolId(Id::Daycare), 1);
                EXPECT_EQ(person->GetPoolId(Id::PreSchool), 1);
        }
}

TEST(GeoGridJSONReaderTest, peopleParsedCorrectlyTest) { runPeopleTest("test2.json"); }

TEST(GeoGridJSONReaderTest, integerParsedCorrectlyTest) { runPeopleTest("test3.json"); }

TEST(GeoGridJSONReaderTest, emptyStreamCorrectExceptionTest)
{
        auto              instream = make_unique<istringstream>("");
        auto              pop      = Population::Create();
        GeoGridJSONReader geoGridJSONReader(move(instream), pop.get());
        EXPECT_THROW(geoGridJSONReader.Read(), Exception);
}

TEST(GeoGridJSONReaderTest, invalidTypeCorrectExceptionTest)
{
        auto pop = Population::Create();
        EXPECT_THROW(getGeoGridFromFile("test4.json", pop.get()), Exception);
}

TEST(GeoGridJSONReaderTest, invalidPersonCorrectExceptionTest)
{
        auto pop = Population::Create();
        EXPECT_THROW(getGeoGridFromFile("test5.json", pop.get()), Exception);
}

TEST(GeoGridJSONReaderTest, invalidJSONCorrectExceptionTest)
{
        auto pop = Population::Create();
        EXPECT_THROW(getGeoGridFromFile("test6.json", pop.get()), Exception);
}

} // namespace
