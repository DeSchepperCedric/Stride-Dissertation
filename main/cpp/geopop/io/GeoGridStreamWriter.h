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

#include "GeoGridWriter.h"

#include <ostream>

namespace stride {
class ContactPool;
class Person;
} // namespace stride

namespace geopop {

class GeoGridStreamWriter : public GeoGridWriter
{
public:
        ///
        explicit GeoGridStreamWriter(std::ostream& stream) : m_stream(stream) {};

        ///
        virtual ~GeoGridStreamWriter() = default;

        ///
        void Write(GeoGrid& geoGrid) override = 0;

        ///
        std::ostream& StreamRef() {return m_stream;}

private:
        std::ostream& m_stream;
};

} // namespace geopop
