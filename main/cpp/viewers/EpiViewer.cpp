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
#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"

#include "../geopop/io/EpiWriterFactory.h"

#include <memory>

using namespace std;
using namespace stride::sim_event;

namespace stride {
namespace viewers {
void EpiViewer::Update(stride::sim_event::Id id)
{
        switch (id) {
        case Id::AtStart: {
                auto& geo = m_runner->GetSim()->GetPopulation()->RefGeoGrid();

                    for(shared_ptr<geopop::Location>& loc : geo){
                        visualization::Location location;
                        location.id               = loc->GetID();
                        location.name             = loc->GetName();
                        location.longitude        = loc->GetCoordinate().get<0>();
                        location.latitude         = loc->GetCoordinate().get<1>();
                        location.size             = loc->GetPopCount();
                        m_Locations[loc->GetID()] = location;
                }
        }
        case Id::Stepped: {
                auto& geo = m_runner->GetSim()->GetPopulation()->RefGeoGrid();
                for (auto& loc : geo) {
                        m_Locations[loc->GetID()].infected.push_back(loc->GetInfectedCount());
                }
                break;
        }
        case Id::Finished: {
                ofstream                      outputFileStream("/home/wannes/temp.json");
                shared_ptr<geopop::EpiWriter> writer =
                    geopop::EpiWriterFactory::CreateEpiWriter("temp.json", outputFileStream);
                writer->Write(m_Locations);
                outputFileStream.close();
        }
        default: break;
        }
}

} // namespace viewers
} // namespace stride
