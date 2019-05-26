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

class DaycareGeneratorTest : public testing::Test
{
public:
        DaycareGeneratorTest()
            : m_rn_man(RnInfo()), m_daycare_generator(m_rn_man), m_gg_config(), m_pop(Population::Create()),
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
        DaycareGenerator       m_daycare_generator;
        GeoGridConfig          m_gg_config;
        shared_ptr<Population> m_pop;
        GeoGrid                m_geo_grid;
        unsigned int           m_ppday = m_gg_config.pools[Id::Daycare];
};

TEST_F(DaycareGeneratorTest, OneLocationTest)
{

        GeoGridConfig::Param param;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4]                     = info;
        m_gg_config.params.at(4).pop_size              = 10000;
        m_gg_config.regionsInfo.at(4).fraction_daycare = 300.0 / m_gg_config.params.at(4).pop_size;

        auto loc1  = make_shared<Location>(1, 4, "Antwerpen", m_gg_config.params.at(4).pop_size);
        auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0, 0));
        m_geo_grid.addLocation(loc1, coor1);

        m_daycare_generator.Apply(m_geo_grid, m_gg_config);

        const auto& poolsOfLoc1 = loc1->CRefPools<Id::Daycare>();
        EXPECT_EQ(poolsOfLoc1.size(), 17 * m_ppday);
}

TEST_F(DaycareGeneratorTest, ZeroLocationTest)
{
        GeoGridConfig::Param param;
        m_gg_config.params[0] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[0] = info;

        m_gg_config.params.at(0).pop_size              = 10000;
        m_gg_config.regionsInfo.at(0).fraction_daycare = 300.0 / m_gg_config.params.at(0).pop_size;

        m_daycare_generator.Apply(m_geo_grid, m_gg_config);

        EXPECT_EQ(m_geo_grid.size(), 0);
}

TEST_F(DaycareGeneratorTest, FiveLocationsTest)
{
        GeoGridConfig::Param param;
        m_gg_config.params[4] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[4] = info;

        m_gg_config.params.at(4).pop_size                    = 35042 * 100;
        m_gg_config.regionsInfo.at(4).fraction_daycare       = 125140.0 / m_gg_config.params.at(4).pop_size;
        m_gg_config.regionsInfo.at(4).major_fraction_daycare = 125140.0 / m_gg_config.params.at(4).pop_size;

        auto loc1  = make_shared<Location>(1, 4, "Antwerpen", 10150 * 100, true);
        auto coor1 = make_shared<EnhancedCoordinate>(loc1.get(), Coordinate(0, 0));
        auto loc2  = make_shared<Location>(1, 4, "Vlaams-Brabant", 10040 * 100);
        auto coor2 = make_shared<EnhancedCoordinate>(loc2.get(), Coordinate(0, 0));
        auto loc3  = make_shared<Location>(1, 4, "Henegouwen", 7460 * 100);
        auto coor3 = make_shared<EnhancedCoordinate>(loc3.get(), Coordinate(0, 0));
        auto loc4  = make_shared<Location>(1, 4, "Limburg", 3269 * 100);
        auto coor4 = make_shared<EnhancedCoordinate>(loc4.get(), Coordinate(0, 0));
        auto loc5  = make_shared<Location>(1, 4, "Luxemburg", 4123 * 100);
        auto coor5 = make_shared<EnhancedCoordinate>(loc5.get(), Coordinate(0, 0));

        m_geo_grid.addLocation(loc1, coor1);
        m_geo_grid.addLocation(loc2, coor2);
        m_geo_grid.addLocation(loc3, coor3);
        m_geo_grid.addLocation(loc4, coor4);
        m_geo_grid.addLocation(loc5, coor5);

        for (const auto& loc : m_geo_grid) {
                loc->SetPopFraction(static_cast<double>(loc->GetPopCount()) /
                                    static_cast<double>(m_gg_config.params.at(4).pop_size));
        }

        m_daycare_generator.Apply(m_geo_grid, m_gg_config);

        array<unsigned int, 5> sizes{2014, 2012, 1456, 679, 792};
        for (auto i = 0U; i < sizes.size(); i++) {
                EXPECT_EQ(sizes[i] * m_ppday, m_geo_grid[i]->CRefPools(Id::Daycare).size());
        }
}

} // namespace
