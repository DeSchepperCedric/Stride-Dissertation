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

#include "GeoGridStreamReader.h"
#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "geopop/Location.h"
#include "geopop/EnhancedCoordinate.h"

#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>

namespace geopop {

class GeoGrid;

/**
 * An implementation of the GeoGridReader using JSON.
 * This class is used to read a GeoGrid from a JSON file.
 */
class GeoGridJSONReader : public GeoGridStreamReader
{
public:
        /// Construct the GeoGridJSONReader with the istream which contains the JSON.
        GeoGridJSONReader(std::unique_ptr<std::istream> inputStream, stride::Population* pop);

        /// No copy constructor.
        GeoGridJSONReader(const GeoGridJSONReader&) = delete;

        /// No copy assignement.
        GeoGridJSONReader operator=(const GeoGridJSONReader&) = delete;

        /// Actually perform the read and return the GeoGrid.
        void Read() override;

private:
        /// Parse all the contactpools of a certain class (type)
        void ParseContactPoolsClass(nlohmann::json& contactCenter, std::shared_ptr<Location> loc);

        /// Create a ContactCenter based on the information stored in the provided json structure
        std::pair<std::shared_ptr<geopop::Location>, std::shared_ptr<geopop::EnhancedCoordinate>> ParseLocation(nlohmann::json& location);

        /// Create a ContactPool based on the information stored in the provided json structure.
        void ParseContactPool(std::shared_ptr<Location> loc, nlohmann::json& contactPool,
                              stride::ContactType::Id typeId);

        /// Create a Coordinate based on the information stored in the provided json structure.
        Coordinate ParseCoordinate(nlohmann::json& coordinate);

        /// Create a Person based on the information stored in the provided json structure.
        stride::Person* ParsePerson(nlohmann::json& person);

        /// Get numerical data from a json node, this will not fail in case it is formatted as a string
        template <typename T>
        T ParseNumerical(nlohmann::json& node)
        {
                if (node.type() == nlohmann::json::value_t::string) {
                        //                        OPTIONAL: log a warning, then we need to get hold of the logger though
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
