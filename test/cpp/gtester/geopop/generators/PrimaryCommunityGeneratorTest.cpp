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
 *  Copyright 2019, Jan Broeckhove.
 */

#include "geopop/generators/Generator.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <array>
#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

class PrimaryCommunityGeneratorTest : public testing::Test
{
public:
        PrimaryCommunityGeneratorTest()
            : m_rn_man(RnInfo()), m_community_generator(m_rn_man), m_gg_config(), m_pop(Population::Create()),
              m_geo_grid(m_pop.get())
        {
                //                for (unsigned int i = 0; i < 5; ++i){
                //                        GeoGridConfig::Param param;
                //                        m_gg_config.params[i] = param;
                //                        GeoGridConfig::Info info;
                //                        m_gg_config.regionsInfo[i] = info;
                //                }
        }

protected:
        RnMan                     m_rn_man;
        PrimaryCommunityGenerator m_community_generator;
        GeoGridConfig             m_gg_config;
        shared_ptr<Population>    m_pop;
        GeoGrid                   m_geo_grid;
        unsigned int              m_pppc = m_gg_config.pools[Id::PrimaryCommunity];
};

TEST_F(PrimaryCommunityGeneratorTest, OneLocationTest)
{
        GeoGridConfig::Param param;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4] = info;

        m_gg_config.params.at(4).pop_size = 10000;

        auto loc1 = make_shared<Location>(1, 4, "Antwerpen", 10150 * 100);
        auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0,0));
        m_geo_grid.addLocation(loc1, coor1);

        const auto& p1 = loc1->CRefPools(Id::PrimaryCommunity);
        EXPECT_EQ(p1.size(), 0);

        m_community_generator.Apply(m_geo_grid, m_gg_config);

        EXPECT_EQ(p1.size(), 5 * m_pppc);
}

TEST_F(PrimaryCommunityGeneratorTest, EqualLocationTest)
{
        GeoGridConfig::Param param;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4] = info;

        m_gg_config.params.at(4).pop_size = 100 * 100 * 1000;

        for (int i = 0; i < 10; i++) {
                auto loc1 = make_shared<Location>(1, 4, "Location " + to_string(i), 10 * 1000 * 1000);
                auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0,0));
                m_geo_grid.addLocation(loc1, coor1);
        }

        m_community_generator.Apply(m_geo_grid, m_gg_config);

        array<unsigned int, 10> expected{546, 495, 475, 500, 463, 533, 472, 539, 496, 481};
        for (auto i = 0U; i < expected.size(); i++) {
                const auto& p = m_geo_grid[i]->CRefPools(Id::PrimaryCommunity);
                EXPECT_EQ(expected[i] * m_pppc, p.size());
        }
}

// Check can handle empty GeoGrid.
TEST_F(PrimaryCommunityGeneratorTest, ZeroLocationTest)
{
        GeoGridConfig::Param param;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4] = info;

        m_gg_config.params.at(4).pop_size = 10000;

        m_community_generator.Apply(m_geo_grid, m_gg_config);

        EXPECT_EQ(m_geo_grid.size(), 0);
}

// Check for five Locations.
TEST_F(PrimaryCommunityGeneratorTest, FiveLocationsTest)
{
        GeoGridConfig::Param param;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4] = info;

        m_gg_config.params.at(4).pop_size                = 37542 * 100;
        m_gg_config.regionsInfo.at(4).fraction_k12school = 750840.0 / m_gg_config.params.at(4).pop_size;

        auto loc1 = make_shared<Location>(1, 4, "Antwerpen", 10150 * 100);
        auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0,0));
        auto loc2 = make_shared<Location>(2, 4, "Vlaams-Brabant", 10040 * 100);
        auto coor2 = make_shared<EnhancedCoordinate>(loc2.get(), Coordinate(0,0));
        auto loc3 = make_shared<Location>(3, 4, "Henegouwen", 7460 * 100);
        auto coor3 = make_shared<EnhancedCoordinate>(loc3.get(), Coordinate(0,0));
        auto loc4 = make_shared<Location>(4, 4, "Limburg", 3269 * 100);
        auto coor4 = make_shared<EnhancedCoordinate>(loc4.get(), Coordinate(0,0));
        auto loc5 = make_shared<Location>(5, 4, "Luxemburg", 4123 * 100);
        auto coor5 = make_shared<EnhancedCoordinate>(loc5.get(), Coordinate(0,0));

        m_geo_grid.addLocation(loc1, coor1);
        m_geo_grid.addLocation(loc2, coor2);
        m_geo_grid.addLocation(loc3, coor3);
        m_geo_grid.addLocation(loc4, coor4);
        m_geo_grid.addLocation(loc5, coor5);

        m_community_generator.Apply(m_geo_grid, m_gg_config);

        array<unsigned int, 5> expected{553, 518, 410, 173, 224};
        for (auto i = 0U; i < expected.size(); i++) {
                const auto& cp = m_geo_grid[i]->CRefPools(Id::PrimaryCommunity);
                EXPECT_EQ(expected[i] * m_pppc, cp.size());
        }
}

} // namespace