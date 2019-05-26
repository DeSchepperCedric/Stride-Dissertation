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

#include "geopop/populators/Populator.h"
#include <gtest/gtest.h>

#include "MakeGeoGrid.h"
#include "contact/AgeBrackets.h"
#include "geopop/Location.h"
#include "geopop/generators/Generator.h"
#include "pop/Population.h"
#include "util/RnMan.h"

#include <gtest/gtest.h>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

namespace {
class WorkplacePopulatorDistributionTest : public testing::Test
{
public:
        WorkplacePopulatorDistributionTest()
            : m_rn_man(RnInfo()), m_workplace_populator(m_rn_man), m_gg_config(), m_pop(Population::Create()),
              m_geo_grid(m_pop->RefGeoGrid()), m_workplace_generator(m_rn_man)
        {
        }

protected:
        RnMan                  m_rn_man;
        WorkplacePopulator     m_workplace_populator;
        GeoGridConfig          m_gg_config;
        shared_ptr<Population> m_pop;
        GeoGrid&               m_geo_grid;
        WorkplaceGenerator     m_workplace_generator;
        const unsigned int     m_ppwp = m_gg_config.pools[Id::Workplace];
};

TEST_F(WorkplacePopulatorDistributionTest, NoCommuting)
{
        MakeGeoGrid(m_gg_config, 3, 1000, 12, 2, 3, 33, 3, m_pop.get());

        m_gg_config.params[1]      = GeoGridConfig::Param{};
        m_gg_config.regionsInfo[1] = GeoGridConfig::Info{};

        m_gg_config.params.at(1).fraction_workplace_commuters = 0;
        m_gg_config.params.at(1).fraction_college_commuters   = 0;
        m_gg_config.regionsInfo.at(1).fraction_workplace      = 0.01;
        m_gg_config.params.at(1).participation_workplace      = 1;
        m_gg_config.params.at(1).participation_college        = 0.5;

        m_gg_config.refWP.ratios = {0.780, 0.171, 0.041, 0.008};
        m_gg_config.refWP.min    = {1, 6, 13, 26};
        m_gg_config.refWP.max    = {5, 12, 25, 50};

        auto schoten = *(m_geo_grid.m_locationGrid->begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto kortrijk = *(m_geo_grid.m_locationGrid->begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        double averageWorkplaceSize = 0.0;

        if (!m_gg_config.refWP.ratios.empty()) {
                for (unsigned int i = 0; i < m_gg_config.refWP.ratios.size(); i++) {
                        averageWorkplaceSize +=
                            m_gg_config.refWP.ratios[i] * (m_gg_config.refWP.max[i] + m_gg_config.refWP.min[i]) / 2;
                }
        }

        const auto WorkplacesCount = static_cast<unsigned int>(ceil(200 / averageWorkplaceSize));

        for (unsigned int k = 0; k < WorkplacesCount; k++) {
                m_workplace_generator.AddPools(*schoten->getData<Location>(), m_pop.get(), m_gg_config);
                m_workplace_generator.AddPools(*kortrijk->getData<Location>(), m_pop.get(), m_gg_config);
        }

        schoten->getData<Location>()->AddOutgoingCommute(kortrijk->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddIncomingCommute(schoten->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddOutgoingCommute(schoten->getData<Location>(), 0.5);
        schoten->getData<Location>()->AddIncomingCommute(kortrijk->getData<Location>(), 0.5);

        m_geo_grid.m_locationGrid->Finalize();

        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        vector<unsigned int> count = {0, 0, 0, 0};

        // count how many workplaces there are of each type
        for (const auto& loc : m_geo_grid) {
                for (const auto& pool : loc->RefPools(Id::Workplace)) {
                        auto size = pool->size();
                        if (size <= m_gg_config.refWP.max[0]) {
                                count[0]++;
                        } else if (size <= m_gg_config.refWP.max[1]) {
                                count[1]++;
                        } else if (size <= m_gg_config.refWP.max[2]) {
                                count[2]++;
                        } else {
                                count[3]++;
                        }
                }
        }

        const vector<unsigned int> expected = {68, 13, 1, 0};

        for (unsigned int i = 0; i < count.size(); i++) {
                EXPECT_EQ(expected[i], count[i]);
        }
}

TEST_F(WorkplacePopulatorDistributionTest, HalfCommuting)
{
        MakeGeoGrid(m_gg_config, 3, 1000, 12, 2, 3, 33, 3, m_pop.get());

        m_gg_config.params[1] = GeoGridConfig::Param{};

        m_gg_config.params.at(1).fraction_workplace_commuters = 0.5;
        m_gg_config.params.at(1).fraction_college_commuters   = 0.5;
        m_gg_config.params.at(1).participation_workplace      = 1;
        m_gg_config.params.at(1).participation_college        = 0.5;

        m_gg_config.regionsInfo[1]                       = GeoGridConfig::Info{};
        m_gg_config.regionsInfo.at(1).fraction_workplace = 0.01;

        m_gg_config.refWP.ratios = {0.760, 0.191, 0.041, 0.008};
        m_gg_config.refWP.min    = {1, 10, 50, 200};
        m_gg_config.refWP.max    = {9, 49, 199, 400};

        auto schoten = *(m_geo_grid.m_locationGrid->begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        auto kortrijk = *(m_geo_grid.m_locationGrid->begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        auto averageWorkplaceSize = 0.0;

        if (!m_gg_config.refWP.ratios.empty()) {
                for (unsigned int i = 0; i < m_gg_config.refWP.ratios.size(); i++) {
                        averageWorkplaceSize +=
                            m_gg_config.refWP.ratios[i] * (m_gg_config.refWP.max[i] + m_gg_config.refWP.min[i]) / 2;
                }
        }

        const auto WorkplacesCount = static_cast<unsigned int>(ceil(200 / static_cast<double>(averageWorkplaceSize)));

        for (unsigned int k = 0; k < WorkplacesCount; k++) {
                m_workplace_generator.AddPools(*schoten->getData<Location>(), m_pop.get(), m_gg_config);
                m_workplace_generator.AddPools(*kortrijk->getData<Location>(), m_pop.get(), m_gg_config);
        }

        schoten->getData<Location>()->AddOutgoingCommute(kortrijk->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddIncomingCommute(schoten->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddOutgoingCommute(schoten->getData<Location>(), 0.5);
        schoten->getData<Location>()->AddIncomingCommute(kortrijk->getData<Location>(), 0.5);

        m_geo_grid.m_locationGrid->Finalize();

        m_workplace_generator.Apply(m_geo_grid, m_gg_config);
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        vector<unsigned int> count = {0, 0, 0, 0};

        // count how many workplaces there are of each type
        for (const auto& loc : m_geo_grid) {
                for (const auto& pool : loc->RefPools(Id::Workplace)) {
                        auto size = pool->size();
                        if (size <= m_gg_config.refWP.max[0]) {
                                count[0]++;
                        } else if (size <= m_gg_config.refWP.max[1]) {
                                count[1]++;
                        } else if (size <= m_gg_config.refWP.max[2]) {
                                count[2]++;
                        } else {
                                count[3]++;
                        }
                }
        }

        const vector<unsigned int> expected = {22, 3, 1, 0};

        for (unsigned int i = 0; i < count.size(); i++) {
                EXPECT_EQ(expected[i], count[i]);
        }
}

TEST_F(WorkplacePopulatorDistributionTest, OnlyCommuting)
{

        MakeGeoGrid(m_gg_config, 3, 1000, 12, 2, 3, 33, 3, m_pop.get());

        m_gg_config.params[1] = GeoGridConfig::Param{};

        m_gg_config.params.at(1).fraction_workplace_commuters = 0.5;
        m_gg_config.params.at(1).fraction_college_commuters   = 0.5;
        m_gg_config.params.at(1).participation_workplace      = 1;
        m_gg_config.params.at(1).participation_college        = 0.5;

        m_gg_config.regionsInfo[1]                       = GeoGridConfig::Info{};
        m_gg_config.regionsInfo.at(1).fraction_workplace = 0.01;

        m_gg_config.refWP.ratios = {0.760, 0.191, 0.041, 0.008};
        m_gg_config.refWP.min    = {1, 10, 50, 200};
        m_gg_config.refWP.max    = {9, 49, 199, 400};

        // only commuting

        auto schoten = *(m_geo_grid.m_locationGrid->begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));

        auto kortrijk = *(m_geo_grid.m_locationGrid->begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        schoten->getData<Location>()->AddOutgoingCommute(kortrijk->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddIncomingCommute(schoten->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddOutgoingCommute(schoten->getData<Location>(), 0.5);
        schoten->getData<Location>()->AddIncomingCommute(kortrijk->getData<Location>(), 0.5);

        auto averageWorkplaceSize = 0.0;

        if (!m_gg_config.refWP.ratios.empty()) {
                for (unsigned int i = 0; i < m_gg_config.refWP.ratios.size(); i++) {
                        averageWorkplaceSize +=
                            m_gg_config.refWP.ratios[i] * (m_gg_config.refWP.max[i] + m_gg_config.refWP.min[i]) / 2;
                }
        }

        const auto WorkplacesCount = static_cast<unsigned int>(ceil(200 / static_cast<double>(averageWorkplaceSize)));

        for (unsigned int k = 0; k < WorkplacesCount; k++) {
                m_workplace_generator.AddPools(*schoten->getData<Location>(), m_pop.get(), m_gg_config);
                m_workplace_generator.AddPools(*kortrijk->getData<Location>(), m_pop.get(), m_gg_config);
        }

        m_geo_grid.m_locationGrid->Finalize();
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        vector<unsigned int> count = {0, 0, 0, 0};

        // count how many workplaces there are of each type
        for (const auto& loc : m_geo_grid) {
                for (const auto& pool : loc->RefPools(Id::Workplace)) {
                        auto size = pool->size();
                        if (size <= m_gg_config.refWP.max[0]) {
                                count[0]++;
                        } else if (size <= m_gg_config.refWP.max[1]) {
                                count[1]++;
                        } else if (size <= m_gg_config.refWP.max[2]) {
                                count[2]++;
                        } else {
                                count[3]++;
                        }
                }
        }

        const vector<unsigned int> expected = {21, 2, 1, 0};

        for (unsigned int i = 0; i < count.size(); i++) {
                EXPECT_EQ(expected[i], count[i]);
        }
}

} // namespace
