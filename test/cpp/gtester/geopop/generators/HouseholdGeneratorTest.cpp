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

class HouseholdGeneratorTest : public testing::Test
{
public:
        HouseholdGeneratorTest()
            : m_rn_man(RnInfo()), m_household_generator(m_rn_man), m_gg_config(), m_pop(Population::Create()),
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
        RnMan                  m_rn_man;
        HouseholdGenerator     m_household_generator;
        GeoGridConfig          m_gg_config;
        shared_ptr<Population> m_pop;
        GeoGrid                m_geo_grid;
        unsigned int           m_pph = m_gg_config.pools[Id::Household];
};

// Check that generator can handle situation with a single Location.
TEST_F(HouseholdGeneratorTest, OneLocationTest)
{
        GeoGridConfig::Param param;
        param.pop_size        = 2500;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4]                     = info;
        m_gg_config.regionsInfo.at(4).count_households = 4;

        auto loc1 = make_shared<Location>(1, 4, "Antwerpen", 2500);
        auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0,0));
        m_geo_grid.addLocation(loc1, coor1);

        m_household_generator.Apply(m_geo_grid, m_gg_config);

        const auto& poolsOfLoc1 = loc1->CRefPools<Id::Household>();
        EXPECT_EQ(poolsOfLoc1.size(), 4);
}

// Check that generator can handle "no Locations" situation.
TEST_F(HouseholdGeneratorTest, ZeroLocationTest)
{
        GeoGridConfig::Param param;
        param.pop_size        = 2500;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4] = info;

        m_gg_config.regionsInfo.at(4).count_households = 4;
        m_household_generator.Apply(m_geo_grid, m_gg_config);

        EXPECT_EQ(m_geo_grid.size(), 0);
}

// check that generator can handle five Locations.
TEST_F(HouseholdGeneratorTest, FiveLocationsTest)
{
        GeoGridConfig::Param param;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4]                           = info;
        m_gg_config.regionsInfo.at(4).count_households       = 4000;
        m_gg_config.regionsInfo.at(4).major_count_households = 4000;
        m_gg_config.params.at(4).pop_size                    = 35042 * 100;

        auto loc1 = make_shared<Location>(1, 4, "Antwerpen", 10150 * 100, true);
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

        for (const auto& loc : m_geo_grid) {
                loc->SetPopFraction(static_cast<double>(loc->GetPopCount()) /
                                    static_cast<double>(m_gg_config.params.at(4).pop_size));
        }

        m_household_generator.Apply(m_geo_grid, m_gg_config);

        array<unsigned int, 5> sizes{1159, 1159, 816, 395, 471};
        for (auto i = 0U; i < sizes.size(); i++) {
                EXPECT_EQ(sizes[i] * m_pph, m_geo_grid[i]->CRefPools(Id::Household).size());
        }
}

} // namespace