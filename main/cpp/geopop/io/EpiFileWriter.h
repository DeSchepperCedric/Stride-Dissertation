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

#include "EpiWriter.h"

#include <string>

namespace stride {
class ContactPool;
class Person;
} // namespace stride

namespace geopop {

class EpiFileWriter : public EpiWriter
{
public:
        /// Construct the GeoGridFileWriter.
        explicit EpiFileWriter(const std::string& fileName) : m_file_name(fileName){};

        /// Default desctructor.
        virtual ~EpiFileWriter() = default;

        /// Write the Geogrid to the proved ostream.
        void Write(std::vector<geopop::EnhancedCoordinate> locations) override = 0;

        /// Get the file name.
        std::string GetFileName() { return m_file_name; }

private:
        std::string m_file_name;
};

} // namespace geopop
