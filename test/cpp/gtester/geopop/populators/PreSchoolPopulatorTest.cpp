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

#include "geopop/populators/PreSchoolPopulator.h"
#include "geopop/generators/PreSchoolGenerator.h"

#include "MakeGeoGrid.h"
#include "contact/AgeBrackets.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>
#include <map>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

class PreSchoolPopulatorTest : public testing::Test
{
public:
        PreSchoolPopulatorTest()
            : m_rn_man(RnInfo()), m_preschool_populator(m_rn_man), m_geogrid_config(), m_pop(Population::Create()),
              m_geo_grid(m_pop->RefGeoGrid()), m_preschool_generator(m_rn_man)
        {
                m_geogrid_config.input.participation_preschool = 0.99;
        }

protected:
        RnMan                  m_rn_man;
        PreSchoolPopulator     m_preschool_populator;
        GeoGridConfig          m_geogrid_config;
        shared_ptr<Population> m_pop;
        GeoGrid&               m_geo_grid;
        PreSchoolGenerator     m_preschool_generator;
        const unsigned int     m_pppre = GeoGridConfig{}.pools.pools_per_preschool;
};

TEST_F(PreSchoolPopulatorTest, NoPopulation)
{
        m_geo_grid.AddLocation(make_shared<Location>(0, 0, Coordinate(0.0, 0.0), "", 0));
        m_geo_grid.Finalize();

        EXPECT_NO_THROW(m_preschool_populator.Apply(m_geo_grid, m_geogrid_config));
}

TEST_F(PreSchoolPopulatorTest, OneLocationTest)
{
        MakeGeoGrid(m_geogrid_config, 1, 300, 20, 3, 5, 100, 3, m_pop.get());
        m_geo_grid.Finalize();
        m_preschool_populator.Apply(m_geo_grid, m_geogrid_config);

        map<int, int> usedCapacity{{1, 0},  {2, 3},  {3, 0},  {4, 1},  {5, 0},  {6, 1},  {7, 0},  {8, 0},  {9, 3},
                                   {10, 0}, {11, 1}, {12, 1}, {13, 1}, {14, 0}, {15, 1}, {16, 1}, {17, 0}, {18, 0}};

        auto  location = *m_geo_grid.begin();
        auto& prePools = location->RefPools(Id::PreSchool);
        ASSERT_EQ(prePools.size(), 3 * m_geogrid_config.pools.pools_per_preschool);
        for (auto& pool : prePools) {
                EXPECT_EQ(usedCapacity[pool->GetId()], pool->size());
                for (Person* person : *pool) {
                        EXPECT_LE(person->GetAge(), 6);
                        EXPECT_GE(person->GetAge(), 3);
                }
        }

        map<int, int> persons{
            {0, 0},   {1, 0},    {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},   {7, 0},   {8, 0},    {9, 0},
            {10, 0},  {11, 0},   {12, 0},  {13, 0},  {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},   {19, 16},
            {20, 0},  {21, 0},   {22, 0},  {23, 0},  {24, 0},  {25, 0},  {26, 0},  {27, 0},  {28, 0},   {29, 0},
            {30, 0},  {31, 0},   {32, 0},  {33, 0},  {34, 0},  {35, 0},  {36, 0},  {37, 0},  {38, 0},   {39, 0},
            {40, 9},  {41, 0},   {42, 0},  {43, 0},  {44, 0},  {45, 0},  {46, 0},  {47, 0},  {48, 0},   {49, 0},
            {50, 0},  {51, 0},   {52, 0},  {53, 0},  {54, 0},  {55, 0},  {56, 0},  {57, 0},  {58, 0},   {59, 0},
            {60, 0},  {61, 0},   {62, 0},  {63, 0},  {64, 13}, {65, 0},  {66, 0},  {67, 0},  {68, 0},   {69, 0},
            {70, 0},  {71, 0},   {72, 0},  {73, 0},  {74, 0},  {75, 0},  {76, 0},  {77, 0},  {78, 0},   {79, 0},
            {80, 0},  {81, 0},   {82, 0},  {83, 0},  {84, 0},  {85, 0},  {86, 0},  {87, 0},  {88, 0},   {89, 0},
            {90, 0},  {91, 0},   {92, 0},  {93, 0},  {94, 0},  {95, 0},  {96, 0},  {97, 0},  {98, 0},   {99, 0},
            {100, 0}, {101, 0},  {102, 0}, {103, 0}, {104, 0}, {105, 0}, {106, 0}, {107, 0}, {108, 0},  {109, 0},
            {110, 0}, {111, 0},  {112, 4}, {113, 0}, {114, 0}, {115, 0}, {116, 2}, {117, 0}, {118, 0},  {119, 0},
            {120, 0}, {121, 0},  {122, 0}, {123, 0}, {124, 0}, {125, 0}, {126, 0}, {127, 0}, {128, 0},  {129, 0},
            {130, 0}, {131, 0},  {132, 0}, {133, 0}, {134, 0}, {135, 0}, {136, 0}, {137, 6}, {138, 0},  {139, 0},
            {140, 0}, {141, 0},  {142, 0}, {143, 0}, {144, 0}, {145, 0}, {146, 0}, {147, 0}, {148, 0},  {149, 0},
            {150, 0}, {151, 0},  {152, 0}, {153, 0}, {154, 0}, {155, 0}, {156, 0}, {157, 0}, {158, 11}, {159, 0},
            {160, 0}, {161, 0},  {162, 0}, {163, 0}, {164, 0}, {165, 0}, {166, 0}, {167, 0}, {168, 0},  {169, 0},
            {170, 0}, {171, 0},  {172, 0}, {173, 0}, {174, 0}, {175, 0}, {176, 0}, {177, 0}, {178, 0},  {179, 0},
            {180, 0}, {181, 0},  {182, 0}, {183, 0}, {184, 0}, {185, 0}, {186, 0}, {187, 0}, {188, 0},  {189, 0},
            {190, 0}, {191, 0},  {192, 0}, {193, 0}, {194, 0}, {195, 0}, {196, 0}, {197, 0}, {198, 0},  {199, 0},
            {200, 0}, {201, 0},  {202, 0}, {203, 2}, {204, 0}, {205, 0}, {206, 0}, {207, 0}, {208, 0},  {209, 0},
            {210, 0}, {211, 0},  {212, 0}, {213, 0}, {214, 9}, {215, 0}, {216, 0}, {217, 0}, {218, 0},  {219, 0},
            {220, 0}, {221, 2},  {222, 0}, {223, 0}, {224, 0}, {225, 0}, {226, 0}, {227, 0}, {228, 0},  {229, 0},
            {230, 0}, {231, 0},  {232, 0}, {233, 0}, {234, 0}, {235, 0}, {236, 0}, {237, 0}, {238, 0},  {239, 0},
            {240, 0}, {241, 0},  {242, 0}, {243, 0}, {244, 0}, {245, 0}, {246, 0}, {247, 0}, {248, 0},  {249, 0},
            {250, 0}, {251, 0},  {252, 0}, {253, 0}, {254, 9}, {255, 0}, {256, 0}, {257, 0}, {258, 0},  {259, 0},
            {260, 0}, {261, 0},  {262, 0}, {263, 0}, {264, 0}, {265, 0}, {266, 0}, {267, 0}, {268, 0},  {269, 0},
            {270, 0}, {271, 15}, {272, 0}, {273, 0}, {274, 0}, {275, 0}, {276, 0}, {277, 0}, {278, 0},  {279, 0},
            {280, 0}, {281, 0},  {282, 0}, {283, 0}, {284, 0}, {285, 0}, {286, 0}, {287, 0}, {288, 0},  {289, 0},
            {290, 0}, {291, 0},  {292, 0}, {293, 0}, {294, 0}, {295, 0}, {296, 0}, {297, 0}, {298, 12}, {299, 0}};

        for (const auto& person : *m_geo_grid.GetPopulation()) {
                EXPECT_EQ(persons[person.GetId()], person.GetPoolId(Id::PreSchool));
        }
}

TEST_F(PreSchoolPopulatorTest, TwoLocationTest)
{
        MakeGeoGrid(m_geogrid_config, 3, 100, 12, 2, 3, 33, 3, m_pop.get());

        // Brasschaat and Schoten are close to each other and will both have students from both.
        // Kortrijk will only have students going to Kortrijk.

        auto brasschaat = *m_geo_grid.begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        auto schoten = *(m_geo_grid.begin() + 1);

        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto kortrijk = *(m_geo_grid.begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        m_geo_grid.Finalize();
        m_preschool_populator.Apply(m_geo_grid, m_geogrid_config);

        auto& prePools1 = brasschaat->RefPools(Id::PreSchool);
        auto& prePools2 = schoten->RefPools(Id::PreSchool);
        auto& prePools3 = kortrijk->RefPools(Id::PreSchool);

        // Check number of pools corresponding to 3 PreSchools per location.
        EXPECT_EQ(prePools1.size(), 2 * m_geogrid_config.pools.pools_per_preschool);
        EXPECT_EQ(prePools2.size(), 2 * m_geogrid_config.pools.pools_per_preschool);
        EXPECT_EQ(prePools3.size(), 2 * m_geogrid_config.pools.pools_per_preschool);

        map<int, int> persons{
            {0, 0},   {1, 0},    {2, 0},    {3, 0},    {4, 0},    {5, 0},   {6, 0},    {7, 0},    {8, 0},   {9, 0},
            {10, 0},  {11, 0},   {12, 0},   {13, 0},   {14, 0},   {15, 0},  {16, 0},   {17, 0},   {18, 0},  {19, 9},
            {20, 0},  {21, 0},   {22, 0},   {23, 0},   {24, 0},   {25, 0},  {26, 0},   {27, 0},   {28, 0},  {29, 0},
            {30, 0},  {31, 0},   {32, 0},   {33, 0},   {34, 0},   {35, 0},  {36, 0},   {37, 0},   {38, 0},  {39, 0},
            {40, 24}, {41, 0},   {42, 0},   {43, 0},   {44, 0},   {45, 0},  {46, 0},   {47, 0},   {48, 0},  {49, 0},
            {50, 0},  {51, 0},   {52, 0},   {53, 0},   {54, 0},   {55, 0},  {56, 0},   {57, 0},   {58, 0},  {59, 0},
            {60, 0},  {61, 0},   {62, 0},   {63, 0},   {64, 5},   {65, 0},  {66, 0},   {67, 0},   {68, 0},  {69, 0},
            {70, 0},  {71, 0},   {72, 0},   {73, 0},   {74, 0},   {75, 0},  {76, 0},   {77, 0},   {78, 0},  {79, 0},
            {80, 0},  {81, 0},   {82, 0},   {83, 0},   {84, 0},   {85, 0},  {86, 0},   {87, 0},   {88, 0},  {89, 0},
            {90, 0},  {91, 0},   {92, 0},   {93, 0},   {94, 0},   {95, 0},  {96, 0},   {97, 0},   {98, 0},  {99, 0},
            {100, 0}, {101, 0},  {102, 0},  {103, 0},  {104, 0},  {105, 0}, {106, 0},  {107, 0},  {108, 0}, {109, 0},
            {110, 0}, {111, 0},  {112, 17}, {113, 0},  {114, 0},  {115, 0}, {116, 14}, {117, 0},  {118, 0}, {119, 0},
            {120, 0}, {121, 0},  {122, 0},  {123, 0},  {124, 0},  {125, 0}, {126, 0},  {127, 0},  {128, 0}, {129, 0},
            {130, 0}, {131, 0},  {132, 0},  {133, 0},  {134, 0},  {135, 0}, {136, 0},  {137, 20}, {138, 0}, {139, 0},
            {140, 0}, {141, 0},  {142, 0},  {143, 0},  {144, 0},  {145, 0}, {146, 0},  {147, 0},  {148, 0}, {149, 0},
            {150, 0}, {151, 0},  {152, 0},  {153, 0},  {154, 0},  {155, 0}, {156, 0},  {157, 0},  {158, 2}, {159, 0},
            {160, 0}, {161, 0},  {162, 0},  {163, 0},  {164, 0},  {165, 0}, {166, 0},  {167, 0},  {168, 0}, {169, 0},
            {170, 0}, {171, 0},  {172, 0},  {173, 0},  {174, 0},  {175, 0}, {176, 0},  {177, 0},  {178, 0}, {179, 0},
            {180, 0}, {181, 0},  {182, 0},  {183, 0},  {184, 0},  {185, 0}, {186, 0},  {187, 0},  {188, 0}, {189, 0},
            {190, 0}, {191, 0},  {192, 0},  {193, 0},  {194, 0},  {195, 0}, {196, 0},  {197, 0},  {198, 0}, {199, 0},
            {200, 0}, {201, 0},  {202, 0},  {203, 25}, {204, 0},  {205, 0}, {206, 0},  {207, 0},  {208, 0}, {209, 0},
            {210, 0}, {211, 0},  {212, 0},  {213, 0},  {214, 30}, {215, 0}, {216, 0},  {217, 0},  {218, 0}, {219, 0},
            {220, 0}, {221, 26}, {222, 0},  {223, 0},  {224, 0},  {225, 0}, {226, 0},  {227, 0},  {228, 0}, {229, 0},
            {230, 0}, {231, 0},  {232, 0},  {233, 0},  {234, 0},  {235, 0}, {236, 0},  {237, 0},  {238, 0}, {239, 0},
            {240, 0}, {241, 0},  {242, 0},  {243, 0},  {244, 0},  {245, 0}, {246, 0},  {247, 0},  {248, 0}, {249, 0},
            {250, 0}, {251, 0},  {252, 0},  {253, 0},  {254, 30}, {255, 0}, {256, 0},  {257, 0},  {258, 0}, {259, 0},
            {260, 0}, {261, 0},  {262, 0},  {263, 0},  {264, 0},  {265, 0}, {266, 0},  {267, 0},  {268, 0}, {269, 0},
            {270, 0}, {271, 34}, {272, 0},  {273, 0},  {274, 0},  {275, 0}, {276, 0},  {277, 0},  {278, 0}, {279, 0},
            {280, 0}, {281, 0},  {282, 0},  {283, 0},  {284, 0},  {285, 0}, {286, 0},  {287, 0},  {288, 0}, {289, 0},
            {290, 0}, {291, 0},  {292, 0},  {293, 0},  {294, 0},  {295, 0}, {296, 0}};

        for (const auto& person : *m_pop) {
                EXPECT_EQ(persons[person.GetId()], person.GetPoolId(Id::PreSchool));
        }

        for (const auto& pool : prePools1) {
                for (const auto& p : *pool) {
                        const auto preId = p->GetPoolId(Id::PreSchool);
                        if (AgeBrackets::PreSchool::HasAge(p->GetAge())) {
                                EXPECT_NE(0, preId);
                        } else {
                                EXPECT_EQ(0, preId);
                        }
                }
        }

        for (const auto& pool : prePools2) {
                for (const auto& p : *pool) {
                        const auto preId = p->GetPoolId(Id::PreSchool);
                        if (AgeBrackets::PreSchool::HasAge(p->GetAge())) {
                                EXPECT_NE(0, preId);
                        } else {
                                EXPECT_EQ(0, preId);
                        }
                }
        }

        for (const auto& pool : prePools3) {
                for (const auto& p : *pool) {
                        const auto preId = p->GetPoolId(Id::PreSchool);
                        if (AgeBrackets::PreSchool::HasAge(p->GetAge())) {
                                EXPECT_NE(0, preId);
                        } else {
                                EXPECT_EQ(0, preId);
                        }
                }
        }
}

} // namespace
