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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Initialize populations: implementation.
 */

#include "GeoPopBuilder.h"

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/generators/Generator.h"
#include "geopop/io/ReaderFactory.h"
#include "geopop/populators/Populator.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"

#include "util/RnMan.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <contact/ContactType.h>
#include <spdlog/logger.h>

namespace stride {

using namespace std;
using namespace util;
using namespace ContactType;
using namespace boost::property_tree;
using namespace geopop;

GeoPopBuilder::GeoPopBuilder(const ptree& config, RnMan& rnMan, shared_ptr<spdlog::logger> strideLogger)
    : AbstractPopBuilder(config, rnMan, move(strideLogger))
{
}

shared_ptr<Population> GeoPopBuilder::Build(shared_ptr<Population> pop)
{
        m_stride_logger->trace("Building geopop.");

        // ------------------------------------------------------------
        // Set the GeoGridConfig.
        // ------------------------------------------------------------
        GeoGridConfig ggConfig(m_config);
        auto   geopop_gen = m_config.get_child("run.geopop_gen");
        ggConfig.SetData(geopop_gen);

        // ------------------------------------------------------------
        // Get GeoGrid associated with 'pop'.
        // ------------------------------------------------------------
        auto& geoGrid = pop->RefGeoGrid();

        // ------------------------------------------------------------
        // Read locations file (commute file only if present).
        // ------------------------------------------------------------
        const auto number_regions = geopop_gen.get<unsigned int>("number_regions");
        for (unsigned int i = 0; i < number_regions; ++i) {
                string commutesFile;
                string citiesFile;

                if (geopop_gen.count("region" + std::to_string(i) + ".commuting_file")) {
                    commutesFile = geopop_gen.get<string>("region" + std::to_string(i) + ".commuting_file");
                } else if (geopop_gen.count("region0.commuting_file")) {
                    commutesFile = geopop_gen.get<string>("region0.commuting_file");
                }
                if (geopop_gen.count("region" + std::to_string(i) + ".cities_file")) {
                    citiesFile = geopop_gen.get<string>("region" + std::to_string(i) + ".cities_file");
                } else {
                    citiesFile = geopop_gen.get<string>("region0.cities_file");
                }

                m_stride_logger->trace("Starting MakeLocations");
                MakeLocations(geoGrid, ggConfig, citiesFile, commutesFile);
                m_stride_logger->trace("Finished MakeLocations");
        }
        // ------------------------------------------------------------
        // Generate Geo.
        // ------------------------------------------------------------
        m_stride_logger->trace("Starting MakePools");
        MakePools(geoGrid, ggConfig);
        m_stride_logger->trace("Finished MakePools");

        // ------------------------------------------------------------
        // Generate Pop.
        // ------------------------------------------------------------
        m_stride_logger->trace("Starting MakePersons");
        MakePersons(geoGrid, ggConfig);
        m_stride_logger->trace("Finished MakePersons");

        // ------------------------------------------------------------
        // Done.
        // ------------------------------------------------------------
        m_stride_logger->trace("Done building geopop.");

        return pop;
}

void GeoPopBuilder::MakeLocations(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig, const string& citiesFileName,
                                  const string& commutingFileName)
{
        const auto locationsReader = ReaderFactory::CreateLocationsReader(citiesFileName);
        locationsReader->FillGeoGrid(geoGrid);

        if (!commutingFileName.empty()) {
                const auto commutesReader = ReaderFactory::CreateCommutesReader(commutingFileName);
                commutesReader->FillGeoGrid(geoGrid);
        }

        for (const shared_ptr<Location>& loc : geoGrid) {
                loc->SetPopCount(geoGridConfig.param.pop_size);
        }
        geoGrid.Finalize();
}

void GeoPopBuilder::MakePools(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        K12SchoolGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        CollegeGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        WorkplaceGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PrimaryCommunityGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        SecondaryCommunityGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        HouseholdGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        DaycareGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PreSchoolGenerator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);
}

void GeoPopBuilder::MakePersons(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        HouseholdPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        K12SchoolPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        CollegePopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PrimaryCommunityPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        SecondaryCommunityPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        WorkplacePopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        DaycarePopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);

        PreSchoolPopulator(m_rn_man, m_stride_logger).Apply(geoGrid, geoGridConfig);
}

} // namespace stride
