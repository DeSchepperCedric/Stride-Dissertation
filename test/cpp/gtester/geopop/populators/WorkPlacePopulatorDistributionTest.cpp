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

TEST_F(WorkplacePopulatorDistributionTest, OnlyCommuting)
{
        MakeGeoGrid(m_gg_config, 3, 100, 12, 90, 3, 33, 3, m_pop.get());

        m_gg_config.param.fraction_workplace_commuters = 1;
        m_gg_config.param.fraction_college_commuters   = 0;
        m_gg_config.info.popcount_workplace            = 1;
        m_gg_config.param.particpation_workplace       = 1;
        m_gg_config.param.participation_college        = 0.5;
        m_gg_config.refWP.average_workplace_size       = 16;
        m_gg_config.refWP.ratios                       = {0.780, 0.171, 0.041, 0.008};
        m_gg_config.refWP.min                          = {1, 10, 50, 200};
        m_gg_config.refWP.max                          = {9, 49, 199, 400};
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
        m_workplace_generator.Apply(m_geo_grid, m_gg_config);
        m_workplace_populator.Apply(m_geo_grid, m_gg_config);

        vector<unsigned int>                      count = {0, 0, 0, 0};
        unordered_map<unsigned int, unsigned int> pools;

        unsigned int total_size = 0U;
        for (const auto& loc : m_geo_grid) {
                for (const auto& pool : loc->RefPools(Id::Workplace)) {
                        auto size = pool->size();
                        total_size++;
                        if (size < m_gg_config.refWP.max[0]) {
                                count[0]++;
                        } else if (size < m_gg_config.refWP.max[1]) {
                                count[1]++;
                        } else if (size < m_gg_config.refWP.max[2]) {
                                count[2]++;
                        } else {
                                count[3]++;
                        }
                }
        }

        m_gg_config.refWP.ratios = {0.780, 0.171, 0.041, 0.008};

        for (unsigned int i = 0; i < total_size; i++) {
                const auto lower_bound  = m_gg_config.refWP.ratios[i] - m_gg_config.refWP.ratios[i] * 0.10;
                const auto upper_bound  = m_gg_config.refWP.ratios[i] + m_gg_config.refWP.ratios[i] * 0.10;
                const auto actual_ratio = (float)count[i] / (float)total_size;

                EXPECT_TRUE((actual_ratio >= lower_bound) && (actual_ratio <= upper_bound));
        }
}
} // namespace
