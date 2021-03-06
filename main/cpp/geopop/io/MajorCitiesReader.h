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

//#include "geopop/GeoGrid.h"

#include <istream>
#include <memory>

namespace geopop {

class GeoGrid;

/**
 * Create an abstract Reader that fills a GeoGrid with the major cities info from file.
 * This can be implemented using different input types. Currently CSV is implemented.
 */
class MajorCitiesReader
{
public:
        /// Construct the MajorCitiesReader with an istream with the file content.
        explicit MajorCitiesReader(std::unique_ptr<std::istream> inputStream) : m_inputStream(std::move(inputStream)) {}

        /// Default destructor.
        virtual ~MajorCitiesReader() = default;

        /// Add major city info to the GeoGrid.
        virtual void FillGeoGrid(GeoGrid&) const = 0;

protected:
        std::unique_ptr<std::istream> m_inputStream; ///< The istream with the file content.
};

} // namespace geopop
