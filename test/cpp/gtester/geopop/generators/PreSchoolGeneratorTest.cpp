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

#include "geopop/generators/PreSchoolGenerator.h"

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

class PreSchoolGeneratorTest : public testing::Test
{
public:
        PreSchoolGeneratorTest()
            : m_rn_man(RnInfo()), m_preschool_generator(m_rn_man), m_geogrid_config(), m_pop(Population::Create()),
              m_geo_grid(m_pop.get())
        {
        }

protected:
        RnMan                  m_rn_man;
        PreSchoolGenerator     m_preschool_generator;
        GeoGridConfig          m_geogrid_config;
        shared_ptr<Population> m_pop;
        GeoGrid                m_geo_grid;
};

TEST_F(PreSchoolGeneratorTest, OneLocationTest)
{
        m_geogrid_config.input.pop_size             = 10000;
        m_geogrid_config.popInfo.popcount_preschool = 300;

        auto loc1 = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", m_geogrid_config.input.pop_size);
        m_geo_grid.AddLocation(loc1);

        m_preschool_generator.Apply(m_geo_grid, m_geogrid_config);

        const auto& poolsOfLoc1 = loc1->CRefPools<Id::PreSchool>();
        EXPECT_EQ(poolsOfLoc1.size(), 3 * m_geogrid_config.pools.pools_per_preschool);
}

TEST_F(PreSchoolGeneratorTest, ZeroLocationTest)
{
        m_geogrid_config.input.pop_size             = 10000;
        m_geogrid_config.popInfo.popcount_preschool = 300;

        m_preschool_generator.Apply(m_geo_grid, m_geogrid_config);

        EXPECT_EQ(m_geo_grid.size(), 0);
}

TEST_F(PreSchoolGeneratorTest, FiveLocationsTest)
{
        m_geogrid_config.input.pop_size             = 37542 * 100;
        m_geogrid_config.popInfo.popcount_preschool = 125140;

        auto loc1 = make_shared<Location>(1, 4, Coordinate(0, 0), "Antwerpen", 10150 * 100);
        auto loc2 = make_shared<Location>(1, 4, Coordinate(0, 0), "Vlaams-Brabant", 10040 * 100);
        auto loc3 = make_shared<Location>(1, 4, Coordinate(0, 0), "Henegouwen", 7460 * 100);
        auto loc4 = make_shared<Location>(1, 4, Coordinate(0, 0), "Limburg", 3269 * 100);
        auto loc5 = make_shared<Location>(1, 4, Coordinate(0, 0), "Luxemburg", 4123 * 100);

        m_geo_grid.AddLocation(loc1);
        m_geo_grid.AddLocation(loc2);
        m_geo_grid.AddLocation(loc3);
        m_geo_grid.AddLocation(loc4);
        m_geo_grid.AddLocation(loc5);

        for (const auto& loc : m_geo_grid) {
                loc->SetPopFraction(static_cast<double>(loc->GetPopCount()) /
                                    static_cast<double>(m_geogrid_config.input.pop_size));
        }

        m_preschool_generator.Apply(m_geo_grid, m_geogrid_config);

        array<unsigned int, 5> sizes{295, 284, 240, 97, 127};
        for (auto i = 0U; i < sizes.size(); i++) {
                EXPECT_EQ(sizes[i] * m_geogrid_config.pools.pools_per_preschool,
                          m_geo_grid[i]->CRefPools(Id::PreSchool).size());
        }
}

} // namespace
