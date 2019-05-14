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

class DemographicTest : public testing::Test
{
public:
    DemographicTest()
            : m_rn_man(RnInfo()), m_daycare_generator(m_rn_man), m_preschool_generator(m_rn_man), m_k12school_generator(m_rn_man), m_workplace_generator(m_rn_man), m_college_generator(m_rn_man),
             m_gg_config(), m_pop(Population::Create()), m_geo_grid(m_pop.get())
    {

    }

protected:
    RnMan                  m_rn_man;
    DaycareGenerator       m_daycare_generator;
    PreSchoolGenerator       m_preschool_generator;
    K12SchoolGenerator       m_k12school_generator;
    WorkplaceGenerator       m_workplace_generator;
    CollegeGenerator       m_college_generator;
    GeoGridConfig          m_gg_config;
    shared_ptr<Population> m_pop;
    GeoGrid                m_geo_grid;
};

// Checks whether stride can handle multiple provincess.
TEST_F(DemographicTest, provincesTest)
{

    for (unsigned int i = 0; i < 5; ++i){
        GeoGridConfig::Param param;
        m_gg_config.params[i] = param;
        GeoGridConfig::Info info;
        m_gg_config.regionsInfo[i]                     = info;
        m_gg_config.params.at(i).pop_size              = 10000;
        m_gg_config.regionsInfo.at(i).fraction_daycare = (float) i / 10;
        m_gg_config.regionsInfo.at(i).fraction_preschool = (float) i / 10;
        m_gg_config.regionsInfo.at(i).fraction_k12school = (float) i / 10;
        m_gg_config.regionsInfo.at(i).fraction_workplace = (float) i / 10;

    }

    for (unsigned int i = 0; i < 5; ++i){
        for (unsigned int nrOfLocations = 0; nrOfLocations < 10; ++nrOfLocations) {
            const auto loc = make_shared<Location>(10*i+nrOfLocations, i, Coordinate(0, 0), "loc" + to_string(nrOfLocations), m_gg_config.params.at(i).pop_size/10);
            m_geo_grid.AddLocation(loc);
        }
    }
    for (const auto& loc : m_geo_grid) {
        loc->SetPopFraction(static_cast<double>(loc->GetPopCount()) /
                            static_cast<double>(m_gg_config.params.at(loc->GetProvince()).pop_size));
    }

    m_daycare_generator.Apply(m_geo_grid, m_gg_config);
    m_preschool_generator.Apply(m_geo_grid, m_gg_config);
    m_k12school_generator.Apply(m_geo_grid, m_gg_config);
    m_workplace_generator.Apply(m_geo_grid, m_gg_config);

    unsigned int expected[5][4] = {{0,0,0,0}, {56,48,50,50}, {111,102,100,100}, {167,150,150,150}, {222,198,200,200}};
    for (auto i = 0U; i < 5; i++){
        auto daycares = 0U;
        auto preschools = 0U;
        auto k12schools = 0U;
        auto workplaces = 0U;
        for (auto loc = 0U; loc < 10; ++loc) {
            daycares += m_geo_grid[i * 10 + loc]->CRefPools<Id::Daycare>().size();
            preschools += m_geo_grid[i * 10 + loc]->CRefPools<Id::PreSchool>().size();
            k12schools += m_geo_grid[i * 10 + loc]->CRefPools<Id::K12School>().size();
            workplaces += m_geo_grid[i * 10 + loc]->CRefPools<Id::Workplace>().size();

        }
        EXPECT_EQ(expected[i][0], daycares);
        EXPECT_EQ(expected[i][1], preschools);
        EXPECT_EQ(expected[i][2], k12schools);
        EXPECT_EQ(expected[i][3], workplaces);
    }
}

TEST(DemoGraphicTest, centralCitiesTest) { EXPECT_TRUE(true); }

} // namespace