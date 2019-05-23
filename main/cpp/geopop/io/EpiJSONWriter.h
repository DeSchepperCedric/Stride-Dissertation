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

#include "EpiStreamWriter.h"

#include <nlohmann/json.hpp>
#include <set>

namespace stride {
class ContactPool;
class Person;
} // namespace stride

namespace geopop {
/**
 * Writes a GeoGrid to a JSON file.
 */
class EpiJSONWriter : public EpiStreamWriter
{
public:

        explicit EpiJSONWriter(std::ostream& stream);

        /// Write the provided GeoGrid to the proved ostream in JSON format.
        void Write(std::map<int, visualization::Location> location) override;

private:
        nlohmann::json WriteLocation(const visualization::Location& location);
};
} // namespace geopop
