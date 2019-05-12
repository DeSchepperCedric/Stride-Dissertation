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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "EpiStreamReader.h"
#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "geopop/Location.h"

#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>

#include "../../../../qt/location.h"

namespace geopop {

class GeoGrid;

/**
 * An implementation of the GeoGridReader using JSON.
 * This class is used to read a GeoGrid from a JSON file.
 */
class EpiJSONReader : public EpiStreamReader
{
public:
        /// Construct the GeoGridJSONReader with the istream which contains the JSON.
        EpiJSONReader(std::unique_ptr<std::istream> inputStream);

        /// No copy constructor.
        EpiJSONReader(const EpiJSONReader&) = delete;

        /// No copy assignement.
        EpiJSONReader operator=(const EpiJSONReader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        std::vector<visualization::Location> Read() override;

private:
        visualization::Location parseLocation(nlohmann::json& node);

        /// Get numerical data from a json node, this will not fail in case it is formatted as a string
        template <typename T>
        T ParseNumerical(nlohmann::json& node)
        {
                if (node.type() == nlohmann::json::value_t::string) {
                        return boost::lexical_cast<T>(node.get<std::string>());
                } else {
                        return node.get<T>();
                }
        }

        /// Get an array from a json node, this will not fail if the array is an empty string
        /// this could be the case, given the provided example files (rather safe than segfault).
        nlohmann::json ParseArray(nlohmann::json& node);
};

} // namespace geopop
