#pragma once
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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the DiseaseBuilder class.
 */

#include <boost/property_tree/ptree.hpp>
#include <memory>

namespace spdlog {
class logger;
}

namespace stride {

class Simulator;

/**
 * Builds a simulator (@see Simulator) based a configuration property tree.
 * It
 * \li reads any additional configuration files (disease, contact, ...)
 * \li initializes calendar and random number manager for the simulator
 * \li builds a contact/transmission logger
 * \li builds a population (vector of persons)
 * \li initialize contactpools
 * \li deals with initial immunity and infection in the population
 * \li adds population members to their contact pools
 */
class DiseaseBuilder
{
public:
        /// Initializing DiseaseBuilder.
        DiseaseBuilder(const boost::property_tree::ptree& config_pt, std::shared_ptr<spdlog::logger> logger);

        /// Build the simulator.
        void Build(std::shared_ptr<Simulator> sim);

private:
        /// Build the simulator.
        void Build(const boost::property_tree::ptree& pt_disease, std::shared_ptr<Simulator> sim);

        /// Get the disease configuration data.
        boost::property_tree::ptree ReadDiseasePtree();

private:
        boost::property_tree::ptree     m_config_pt;     ///< Run config in ptree.
        std::shared_ptr<spdlog::logger> m_stride_logger; ///< Stride run logger (!= contact_logger).
};

} // namespace stride
