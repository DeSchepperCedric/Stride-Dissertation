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
 * Observer for Epi output.
 */

#include "../../../qt/location.h"
#include "InfectedFile.h"
#include "sim/event/Id.h"

#include <iostream>
#include <spdlog/spdlog.h>

#ifndef STRIDE_EPIVIEWER_H
#define STRIDE_EPIVIEWER_H

namespace stride {

class SimRunner;

namespace viewers {

class EpiViewer
{
public:
        explicit EpiViewer(std::shared_ptr<SimRunner> runner, std::string filename = "test.json")
            : m_Locations(), m_runner(std::move(runner)), m_filename(std::move(filename))
        {
        }

        ~EpiViewer() = default;

        // Let viewer perform update
        void Update(sim_event::Id id);

private:
        std::map<int, visualization::Location> m_Locations;
        std::shared_ptr<SimRunner>             m_runner;
        std::string                            m_filename;
};
} // namespace viewers
} // namespace stride

#endif // STRIDE_EPIVIEWER_H
