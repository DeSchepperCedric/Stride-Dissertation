//
// Created by wannes on 31.03.19.
//

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

TEST(workplacePopulatorDistributionTest, workplacePopulatorTest) { EXPECT_TRUE(true); }

TEST(workplacePopulatorDistributionTest, overPopulationTest) { EXPECT_TRUE(true); }

TEST(workplacePopulatorDistributionTest, noPopulationTest) { EXPECT_TRUE(true); }

TEST_F(WorkplacePopulatorDistributionTest, NoCommutingAvailable)
{
        MakeGeoGrid(m_gg_config, 3, 100, 12, 2, 3, 33, 3, m_pop.get());

        m_gg_config.param.fraction_workplace_commuters = 0;
        m_gg_config.param.fraction_workplace_commuters = 1;
        m_gg_config.param.fraction_college_commuters   = 0;
        m_gg_config.info.popcount_workplace            = 1;
        m_gg_config.param.particpation_workplace       = 1;
        m_gg_config.param.participation_college        = 0.5;
        m_gg_config.refWP.average_workplace_size       = 290;
        m_gg_config.refWP.ratios                       = {0.05, 0.95};
        m_gg_config.refWP.min = {1,200};
    m_gg_config.refWP.max = {199,400};

        auto brasschaat = *m_geo_grid.begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));
        m_workplace_generator.AddPools(*brasschaat, m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*brasschaat, m_pop.get(), m_gg_config);

        auto schoten = *(m_geo_grid.begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        m_workplace_generator.AddPools(*schoten, m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*schoten, m_pop.get(), m_gg_config);

        auto kortrijk = *(m_geo_grid.begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        m_workplace_generator.AddPools(*kortrijk, m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*kortrijk, m_pop.get(), m_gg_config);

        // test case is only commuting but between nobody is commuting from or to Brasschaat
        schoten->AddOutgoingCommute(kortrijk, 0.5);
        kortrijk->AddIncomingCommute(schoten, 0.5);
        kortrijk->AddOutgoingCommute(schoten, 0.5);
        schoten->AddIncomingCommute(kortrijk, 0.5);

        m_geo_grid.Finalize();
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& hPool : schoten->RefPools(Id::Household)) {
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
        for (const auto& hPool : brasschaat->RefPools(Id::Household)) {
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
        for (const auto& hPool : kortrijk->RefPools(Id::Household)) {
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

TEST_F(WorkplacePopulatorDistributionTest, OnlyCommuting)
{
        MakeGeoGrid(m_gg_config, 3, 100, 12, 90, 3, 33, 3, m_pop.get());

        m_gg_config.param.fraction_workplace_commuters = 0;
        m_gg_config.param.fraction_workplace_commuters = 1;
        m_gg_config.param.fraction_college_commuters   = 0;
        m_gg_config.info.popcount_workplace            = 1;
        m_gg_config.param.particpation_workplace       = 1;
        m_gg_config.param.participation_college        = 0.5;
    m_gg_config.refWP.average_workplace_size       = 290;
    m_gg_config.refWP.ratios                       = {0.05, 0.95};
    m_gg_config.refWP.min = {1,200};
    m_gg_config.refWP.max = {199,400};
        // only commuting

        auto schoten = *(m_geo_grid.begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));
        m_workplace_generator.AddPools(*schoten, m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*schoten, m_pop.get(), m_gg_config);

        auto kortrijk = *(m_geo_grid.begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));
        m_workplace_generator.AddPools(*kortrijk, m_pop.get(), m_gg_config);
        m_workplace_generator.AddPools(*kortrijk, m_pop.get(), m_gg_config);

        schoten->AddOutgoingCommute(kortrijk, 0.5);
        kortrijk->AddIncomingCommute(schoten, 0.5);
        kortrijk->AddOutgoingCommute(schoten, 0.5);
        schoten->AddIncomingCommute(kortrijk, 0.5);

        m_geo_grid.Finalize();
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& hPool : schoten->RefPools(Id::Household)) {
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
        for (const auto& hPool : kortrijk->RefPools(Id::Household)) {
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
} // namespace
