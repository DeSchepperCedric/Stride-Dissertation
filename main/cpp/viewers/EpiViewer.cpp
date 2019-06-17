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
 * Definition of Observer for SimEvents for commandline interface usage.
 */

#include "EpiViewer.h"

#include "geopop/Location.h"
#include "geopop/EnhancedCoordinate.h"
#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"

#include "../geopop/io/EpiWriterFactory.h"

#include <memory>
#include <unordered_map>

using namespace std;
using namespace stride::sim_event;

namespace stride {
namespace viewers {
void EpiViewer::Update(stride::sim_event::Id id)
{
        switch (id) {
        case Id::AtStart: {
                auto& geo = m_runner->GetSim()->GetPopulation()->RefGeoGrid();

                cout << "data:      " << geo.size() << endl;
                for (auto& loc : *geo.m_locationGrid) {
                        visualization::Location location;
                        geopop::EnhancedCoordinate coor = geopop::EnhancedCoordinate(nullptr);
                        location.id               = loc->getData<geopop::Location>()->GetID();
                        location.name             = loc->getData<geopop::Location>()->GetName();
                        coor.SetCoordinate(geopop::Coordinate(loc->GetCoordinate()));
                        location.size             = loc->getData<geopop::Location>()->GetPopCount();
                        coor.setData(&location);
                        const auto& epi = loc->getData<geopop::Location>()->GetStatusCounts();
                        for(const auto& age: epi){
                            for(const auto& status: age.second){
                                location.infected[age.first][status.first].push_back(status.second);
                            }
                        }
                        m_Locations[loc->getData<geopop::Location>()->GetID()] = location;
                }
                break;
        }
        case Id::Stepped: {
                auto& geo = m_runner->GetSim()->GetPopulation()->RefGeoGrid();
                for (auto& loc : geo) {
                    const auto& epi = loc->GetStatusCounts();
                    for(const auto& age: epi){
                        for(const auto& status: age.second){
                            m_Locations[loc->GetID()].infected[age.first][status.first].push_back(status.second);
                        }
                    }
                }
                break;
        }
        case Id::Finished: {
                ofstream                      outputFileStream("temp.json");
                shared_ptr<geopop::EpiWriter> writer =
                    geopop::EpiWriterFactory::CreateEpiWriter("temp.json", outputFileStream);
                writer->Write(m_coors);
                outputFileStream.close();
        }
        default: break;
        }
}

} // namespace viewers
} // namespace stride
