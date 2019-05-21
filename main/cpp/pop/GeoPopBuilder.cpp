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
        auto          geopop_gen = m_config.get_child("run.geopop_gen");
        ggConfig.SetData(geopop_gen);

        // ------------------------------------------------------------
        // Get GeoGrid associated with 'pop'.
        // ------------------------------------------------------------
        auto& geoGrid = pop->RefGeoGrid();

        // ------------------------------------------------------------
        // Read locations file (commute file only if present).
        // ------------------------------------------------------------
        string commutesFile;
        if (geopop_gen.count("commuting_file")) {
                commutesFile = geopop_gen.get<string>("commuting_file");
        }
        string majorCitiesFile;
        if (geopop_gen.count("major_cities_file")) {
                majorCitiesFile = geopop_gen.get<string>("major_cities_file");
        }

        string citiesFile = geopop_gen.get<string>("cities_file");

        m_stride_logger->trace("Starting MakeLocations");
        MakeLocations(geoGrid, ggConfig, citiesFile, commutesFile, majorCitiesFile);
        m_stride_logger->trace("Finished MakeLocations");

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

void GeoPopBuilder::MakeLocations(GeoGrid& geoGrid, GeoGridConfig& geoGridConfig, const string& citiesFileName,
                                  const string& commutingFileName, const string& majorCitiesFileName)
{
        const auto locationsReader = ReaderFactory::CreateLocationsReader(citiesFileName);
        locationsReader->FillGeoGrid(geoGrid);

        // Check if there's a major cities file, if so, parse it and set a boolean in the proper locations
        if (!majorCitiesFileName.empty()) {
                const auto majorCitiesReader = ReaderFactory::CreateMajorCitiesReader(majorCitiesFileName);
                majorCitiesReader->FillGeoGrid(geoGrid);
        }
        if (!commutingFileName.empty()) {
                const auto commutesReader = ReaderFactory::CreateCommutesReader(commutingFileName);
                commutesReader->FillGeoGrid(geoGrid);
        }

        auto total_pop = 0U;
        for (const auto& param : geoGridConfig.params) {
                if (param.first == -1) {
                        continue;
                }
                total_pop += param.second.pop_size;
        }
        if (total_pop == 0) {
                total_pop = geoGridConfig.params.at(-1).pop_size;
        }

        for (const shared_ptr<Location>& loc : geoGrid) {
                if (geoGridConfig.params.find(loc->GetProvince()) == geoGridConfig.params.end()) {
                        geoGridConfig.params[loc->GetProvince()]        = geoGridConfig.params.at(-1);
                        geoGridConfig.regionsInfo[loc->GetProvince()]   = geoGridConfig.regionsInfo.at(-1);
                        geoGridConfig.refHouseHolds[loc->GetProvince()] = geoGridConfig.refHouseHolds.at(-1);
                }
                loc->SetPopCount(total_pop);
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
