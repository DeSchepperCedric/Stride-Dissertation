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

class WorkplacePopulatorTest : public testing::Test
{
public:
        WorkplacePopulatorTest()
            : m_rn_man(RnInfo()), m_workplace_populator(m_rn_man), m_gg_config(), m_pop(Population::Create()),
              m_geo_grid(m_pop->RefGeoGrid()), m_workplace_generator(m_rn_man)
        {
                for (unsigned int i = 0; i < 5; ++i) {
                        GeoGridConfig::Param param;
                        m_gg_config.params[i] = param;
                }
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

TEST_F(WorkplacePopulatorTest, NoPopulation)
{
        auto loc = make_shared<Location>(0,0,"", 0);
        auto coor = make_shared<EnhancedCoordinate>(loc.get(), Coordinate(0.0,0.0));
        m_geo_grid.addLocation(loc, coor);
        m_geo_grid.m_locationGrid->Finalize();

        EXPECT_NO_THROW(m_workplace_populator.Apply(m_geo_grid, m_gg_config));
}

TEST_F(WorkplacePopulatorTest, NoActive)
{
        MakeGeoGrid(m_gg_config, 3, 100, 12, 2, 3, 33, 3, m_pop.get());
        m_gg_config.regionsInfo[1]                       = GeoGridConfig::Info{};
        m_gg_config.params.at(1).participation_workplace = 0;
        m_gg_config.params.at(1).participation_college   = 1;

        // Nobody works, everybody in the student age bracket goes to college: so workplace is empty.
        // Brasschaat and Schoten are close to each other. There is no commuting, but they are so close
        // they will receive students from each other. Kortrijk will only receive students from Kortrijk.
        auto brasschaat = *m_geo_grid.m_locationGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));

        auto schoten = *(m_geo_grid.m_locationGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));

        auto kortrijk = *(m_geo_grid.m_locationGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        m_geo_grid.m_locationGrid->Finalize();
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        for (const Person& p : *m_geo_grid.GetPopulation()) {
                EXPECT_EQ(0, p.GetPoolId(Id::Workplace));
        }
}

TEST_F(WorkplacePopulatorTest, NoCommuting)
{
        MakeGeoGrid(m_gg_config, 3, 100, 12, 2, 3, 33, 3, m_pop.get());
        m_gg_config.regionsInfo[1] = GeoGridConfig::Info{};

        m_gg_config.params.at(1).fraction_workplace_commuters = 0;
        m_gg_config.params.at(1).participation_workplace      = 1;
        m_gg_config.params.at(1).participation_college        = 0.5;

        // Brasschaat and Schoten are close to each other
        // There is no commuting, but since they will still receive students from each other
        // Kortrijk will only receive students from Kortrijik

        auto brasschaat = *m_geo_grid.m_locationGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        m_workplace_generator.AddPools(*brasschaat->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*brasschaat->getData<Location>(), m_pop.get(), m_gg_config);

        auto schoten = *(m_geo_grid.m_locationGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        m_workplace_generator.AddPools(*schoten->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*schoten->getData<Location>(), m_pop.get(), m_gg_config);

        auto kortrijk = *(m_geo_grid.m_locationGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        m_workplace_generator.AddPools(*kortrijk->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*kortrijk->getData<Location>(), m_pop.get(), m_gg_config);

        m_geo_grid.m_locationGrid->Finalize();
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        // Assert that persons of Schoten only go to Schoten or Brasschaat
        for (const auto& hPool : schoten->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 4 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 4 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Brasschaat only go to Schoten or Brasschaat
        for (const auto& hPool : brasschaat->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 4 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 4 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Kortijk
        for (const auto& hPool : kortrijk->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 4 * m_ppwp && workId <= 6 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 4 * m_ppwp && workId <= 6 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }
}

TEST_F(WorkplacePopulatorTest, OnlyCommuting)
{
        MakeGeoGrid(m_gg_config, 3, 100, 12, 90, 3, 33, 3, m_pop.get());
        m_gg_config.regionsInfo[1]                            = GeoGridConfig::Info{};
        m_gg_config.params.at(1).fraction_workplace_commuters = 0;
        m_gg_config.params.at(1).fraction_workplace_commuters = 1;
        m_gg_config.params.at(1).fraction_college_commuters   = 0;
        m_gg_config.regionsInfo.at(1).fraction_workplace      = 0.01;
        m_gg_config.params.at(1).participation_workplace      = 1;
        m_gg_config.params.at(1).participation_college        = 0.5;

        // only commuting

        auto schoten = *(m_geo_grid.m_locationGrid->begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        m_workplace_generator.AddPools(schoten->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(schoten->getData<Location>(), m_pop.get(), m_gg_config);

        auto kortrijk = *(m_geo_grid.m_locationGrid->begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        m_workplace_generator.AddPools(kortrijk->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(kortrijk->getData<Location>(), m_pop.get(), m_gg_config);

        schoten->getData<Location>()->AddOutgoingCommute(kortrijk->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddIncomingCommute(schoten->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddOutgoingCommute(schoten->getData<Location>(), 0.5);
        schoten->getData<Location>()->AddIncomingCommute(kortrijk->getData<Location>(), 0.5);

        m_geo_grid.m_locationGrid->Finalize();
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& hPool : schoten->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 2 * m_ppwp && workId <= 4 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 2 * m_ppwp && workId <= 4 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Schoten
        for (const auto& hPool : kortrijk->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 2 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 2 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }
}

TEST_F(WorkplacePopulatorTest, NoCommutingAvailable)
{
        MakeGeoGrid(m_gg_config, 3, 100, 12, 2, 3, 33, 3, m_pop.get());
        m_gg_config.regionsInfo[1]                            = GeoGridConfig::Info{};
        m_gg_config.params.at(1).fraction_workplace_commuters = 0;
        m_gg_config.params.at(1).fraction_workplace_commuters = 1;
        m_gg_config.params.at(1).fraction_college_commuters   = 0;
        m_gg_config.regionsInfo.at(1).fraction_workplace      = 0.01;
        m_gg_config.params.at(1).participation_workplace      = 1;
        m_gg_config.params.at(1).participation_college        = 0.5;


        auto brasschaat = *m_geo_grid.m_locationGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        m_workplace_generator.AddPools(brasschaat->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(brasschaat->getData<Location>(), m_pop.get(), m_gg_config);

        auto schoten = *(m_geo_grid.m_locationGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        m_workplace_generator.AddPools(schoten->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(schoten->getData<Location>(), m_pop.get(), m_gg_config);

        auto kortrijk = *(m_geo_grid.m_locationGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        m_workplace_generator.AddPools(kortrijk->getData<Location>(), m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(kortrijk->getData<Location>(), m_pop.get(), m_gg_config);

        // test case is only commuting but between nobody is commuting from or to Brasschaat
        schoten->getData<Location>()->AddOutgoingCommute(kortrijk->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddIncomingCommute(schoten->getData<Location>(), 0.5);
        kortrijk->getData<Location>()->AddOutgoingCommute(schoten->getData<Location>(), 0.5);
        schoten->getData<Location>()->AddIncomingCommute(kortrijk->getData<Location>(), 0.5);

        m_geo_grid.m_locationGrid->Finalize();
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& hPool : schoten->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 4 * m_ppwp && workId <= 6 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 4 * m_ppwp && workId <= 6 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Brasschaat only go to Brasschaat or Schoten
        for (const auto& hPool : brasschaat->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId >= 1 && workId <= 4 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId >= 1 && workId <= 4 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Schoten
        for (const auto& hPool : kortrijk->getData<Location>()->RefPools(Id::Household)) {
                for (auto p : hPool[0]) {
                        const auto workId = p->GetPoolId(Id::Workplace);
                        if (AgeBrackets::Workplace::HasAge(p->GetAge()) && !AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE(workId > 2 * m_ppwp && workId <= 4 * m_ppwp);
                        } else if (AgeBrackets::College::HasAge(p->GetAge())) {
                                EXPECT_TRUE((workId > 2 * m_ppwp && workId <= 4 * m_ppwp) || workId == 0);
                        } else {
                                EXPECT_EQ(0, workId);
                        }
                }
        }
}

} // namespace
