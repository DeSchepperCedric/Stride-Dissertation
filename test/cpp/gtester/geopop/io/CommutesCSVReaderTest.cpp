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

#include "geopop/io/CommutesCSVReader.h"
#include "geopop/GeoGrid.h"
#include "geopop/Location.h"
#include "geopop/io/HouseholdCSVReader.h"
#include "pop/Population.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace geopop;
using namespace stride;

namespace {

shared_ptr<Population> getExpectedGeoGrid()
{
        auto  pop     = Population::Create();
        auto& geoGrid = pop->RefGeoGrid();
        auto  loc1    = make_shared<Location>(21, 0, "", 1000);
        auto  coor1   = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0.0, 0.0));
        geoGrid.addLocation(loc1, coor1);
        auto loc2  = make_shared<Location>(22, 0, "", 800);
        auto coor2 = make_shared<EnhancedCoordinate>(loc2.get(), Coordinate(0.0, 0.0));
        geoGrid.addLocation(loc2, coor2);
        auto loc3  = make_shared<Location>(23, 0, "", 900);
        auto coor3 = make_shared<EnhancedCoordinate>(loc3.get(), Coordinate(0.0, 0.0));
        geoGrid.addLocation(loc3, coor3);
        auto loc4  = make_shared<Location>(24, 0, "", 1300);
        auto coor4 = make_shared<EnhancedCoordinate>(loc4.get(), Coordinate(0.0, 0.0));
        geoGrid.addLocation(loc4, coor4);

        // to 21
        geoGrid.GetById(21)->AddIncomingCommute(geoGrid.GetById(22).get(), 0.15012305168170631);
        geoGrid.GetById(22)->AddOutgoingCommute(geoGrid.GetById(21).get(), 0.15012305168170631);

        geoGrid.GetById(21)->AddIncomingCommute(geoGrid.GetById(23).get(), 0.59115044247787607);
        geoGrid.GetById(23)->AddOutgoingCommute(geoGrid.GetById(21).get(), 0.59115044247787607);

        geoGrid.GetById(21)->AddIncomingCommute(geoGrid.GetById(24).get(), 0.37610619469026546);
        geoGrid.GetById(24)->AddOutgoingCommute(geoGrid.GetById(21).get(), 0.37610619469026546);

        // to 22
        geoGrid.GetById(22)->AddIncomingCommute(geoGrid.GetById(21).get(), 0.11969439728353141);
        geoGrid.GetById(21)->AddOutgoingCommute(geoGrid.GetById(22).get(), 0.11969439728353141);

        geoGrid.GetById(22)->AddIncomingCommute(geoGrid.GetById(24).get(), 0.62389380530973448);
        geoGrid.GetById(24)->AddOutgoingCommute(geoGrid.GetById(22).get(), 0.62389380530973448);

        // to 23
        geoGrid.GetById(23)->AddIncomingCommute(geoGrid.GetById(21).get(), 0.41341256366723261);
        geoGrid.GetById(21)->AddOutgoingCommute(geoGrid.GetById(23).get(), 0.41341256366723261);

        geoGrid.GetById(23)->AddIncomingCommute(geoGrid.GetById(22).get(), 0.28712059064807222);
        geoGrid.GetById(22)->AddOutgoingCommute(geoGrid.GetById(23).get(), 0.28712059064807222);

        // to 24
        geoGrid.GetById(24)->AddIncomingCommute(geoGrid.GetById(22).get(), 0.2506152584085316);
        geoGrid.GetById(22)->AddOutgoingCommute(geoGrid.GetById(24).get(), 0.2506152584085316);

        return pop;
}

TEST(CommutesCSVReaderTest, test1)
{
        string csvString = "id_21,id_22,id_23,id_24\n"
                           "550,366,668,425\n" // to 21
                           "141,761,0,705\n"   // to 22
                           "487,700,462,0\n"   // to 23
                           "0,611,0,0\n";      // to 24

        auto       expectedPop     = getExpectedGeoGrid();
        auto&      expectedGeoGrid = expectedPop->RefGeoGrid();
        const auto pop             = Population::Create();
        auto&      grid            = pop->RefGeoGrid();

        auto loc1  = make_shared<Location>(21, 0, "", 1000);
        auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0.0, 0.0));
        grid.addLocation(loc1, coor1);
        auto loc2  = make_shared<Location>(22, 0, "", 800);
        auto coor2 = make_shared<EnhancedCoordinate>(loc2.get(), Coordinate(0.0, 0.0));
        grid.addLocation(loc2, coor2);
        auto loc3  = make_shared<Location>(23, 0, "", 900);
        auto coor3 = make_shared<EnhancedCoordinate>(loc3.get(), Coordinate(0.0, 0.0));
        grid.addLocation(loc3, coor3);
        auto loc4  = make_shared<Location>(24, 0, "", 1300);
        auto coor4 = make_shared<EnhancedCoordinate>(loc4.get(), Coordinate(0.0, 0.0));
        grid.addLocation(loc4, coor4);

        auto              instream = make_unique<istringstream>(csvString);
        CommutesCSVReader reader(move(instream));
        reader.FillGeoGrid(grid);

        for (const auto& loc : grid) {
                int         i                = 0;
                const auto& expectedLoc      = expectedGeoGrid.GetById(loc->GetID());
                const auto& outGoingExpected = expectedLoc->CRefOutgoingCommutes();
                for (const auto& commute : loc->CRefOutgoingCommutes()) {
                        EXPECT_DOUBLE_EQ(outGoingExpected[i].first->GetID(), commute.first->GetID());
                        EXPECT_DOUBLE_EQ(outGoingExpected[i].second, commute.second);
                        i++;
                }
                i                            = 0;
                const auto& incomingExpected = expectedLoc->CRefIncomingCommutes();
                for (const auto& commute : loc->CRefIncomingCommutes()) {
                        EXPECT_DOUBLE_EQ(incomingExpected[i].first->GetID(), commute.first->GetID());
                        EXPECT_DOUBLE_EQ(incomingExpected[i].second, commute.second);
                        i++;
                }

                EXPECT_DOUBLE_EQ(expectedLoc->GetOutgoingCommuteCount(1), loc->GetOutgoingCommuteCount(1));
                EXPECT_DOUBLE_EQ(expectedLoc->GetIncomingCommuteCount(1), loc->GetIncomingCommuteCount(1));
        }
}

} // namespace
