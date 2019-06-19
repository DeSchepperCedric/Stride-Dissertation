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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "EpiHDF5Reader.h"

#include "geopop/Coordinate.h"
#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <iostream>
#include <memory>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

EpiHDF5Reader::EpiHDF5Reader(std::string inputFile) : EpiFileReader(inputFile) {}

std::pair<std::vector<visualization::Location*>, std::vector<geopop::EnhancedCoordinate>> EpiHDF5Reader::Read()
{

        m_inputFile = "temp.h5";

        H5File file;
        H5::Exception::dontPrint();

        try {
                H5::Exception::dontPrint();
                cout << m_inputFile << endl;
                file = H5File(m_inputFile, H5F_ACC_RDONLY);
                std::vector<visualization::Location*>   locs;
                std::vector<geopop::EnhancedCoordinate> coords;

                auto         locations = file.openGroup("locations");
                auto         size      = ReadAttribute<unsigned int>("size", locations);
                const string name      = "location";
                for (unsigned int i = 1; i <= size; ++i) {
                        string location_name = name + to_string(i);
                        auto   location      = locations.openGroup(location_name);
                        auto   loc           = ParseLocation(location);
                        locs.push_back(loc.first);
                        coords.push_back(loc.second);
                }
                file.close();
                return make_pair(locs, coords);

        } catch (FileIException& error) {
                throw util::Exception(error.getDetailMsg());
        } catch (DataSetIException& error) {
                throw util::Exception(error.getDetailMsg());
        } catch (GroupIException& error) {
                throw util::Exception(error.getDetailMsg());
        } catch (runtime_error&) {
                throw util::Exception("Problem parsing HDF5 file, check whether empty or invalid HDF5.");
        }
}

template <typename T>
T EpiHDF5Reader::ReadAttribute(const string& name, H5Object& object)
{
        T         data;
        Attribute attribute = object.openAttribute(name);
        attribute.read(attribute.getDataType(), &data);
        return data;
}

template <>
string EpiHDF5Reader::ReadAttribute(const string& name, H5Object& object)
{
        string    data;
        Attribute attribute = object.openAttribute(name);
        attribute.read(attribute.getStrType(), data);
        return data;
}

std::pair<visualization::Location*, EnhancedCoordinate> EpiHDF5Reader::ParseLocation(H5::Group& location)
{
        auto      id         = ReadAttribute<unsigned int>("id", location);
        string    name       = ReadAttribute<string>("name", location);
        auto      pop        = ReadAttribute<unsigned int>("population", location);
        auto      days       = ReadAttribute<unsigned int>("days", location);
        Attribute attr_coord = location.openAttribute("coordinates");
        double    coord[2];
        attr_coord.read(attr_coord.getDataType(), &coord);
        Coordinate c      = {boost::lexical_cast<double>(coord[1]), boost::lexical_cast<double>(coord[0])};
        auto*      result = new visualization::Location();
        result->size      = pop;
        result->name      = name;
        result->id        = id;

        EnhancedCoordinate coordinate(nullptr);
        coordinate.SetCoordinate(c);
        coordinate.setData(result);

        const auto age    = {"College",   "Daycare",          "Household",          "K12School",
                          "PreSchool", "PrimaryCommunity", "SecondaryCommunity", "Workplace"};
        const auto status = {"immune", "infected", "infectious", "recovered", "susceptible", "symptomatic", "total"};

        for (const auto& a : age) {
                auto ageGroup = location.openGroup(a);
                for (const auto& s : status) {
                        auto                 statusGroup = ageGroup.openGroup(s);
                        vector<unsigned int> infected_days;
                        for (unsigned int i = 0; i < days; i++) {
                                infected_days.push_back(ReadAttribute<unsigned int>(std::to_string(i), statusGroup));
                        }
                        result->infected[a][s] = infected_days;
                }
        }

        return make_pair(result, coordinate);
}

} // namespace geopop
