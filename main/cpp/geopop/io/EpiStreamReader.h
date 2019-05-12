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

#include "EpiReader.h"

#include <istream>

namespace geopop {


/**
 * An abstract class for creating a GeoGrid that was read from a file, can be implemented
 * using multiple file types (proto and json are currently implemented)
 */
class EpiStreamReader : public EpiReader
{
public:
        /// Parametrized constructor.
        explicit EpiStreamReader(std::unique_ptr<std::istream> inputStream) : EpiReader(), m_inputStream(std::move(inputStream)) {};

        /// No copy constructor.
        EpiStreamReader(const EpiStreamReader&) = delete;

        /// No copy assignment.
        EpiStreamReader& operator=(const EpiStreamReader&) = delete;

        /// Default destructor.
        virtual ~EpiStreamReader() = default;

        /// Perform the actual read and return the created GeoGrid.
        virtual std::vector<visualization::Location>  Read() override = 0;

protected:
        std::unique_ptr<std::istream> m_inputStream; ///< File to read.
};

} // namespace geopop
