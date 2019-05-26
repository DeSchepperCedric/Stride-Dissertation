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
 *  Copyright 2019, Jan Broeckhove and Bistromatics group.
 */

#include <string>
#include <map>
#include <vector>
#include <unordered_map>

#ifndef LOCATION_H
#define LOCATION_H


namespace visualization {

    class Location {
    public:
        std::string name = "";
        unsigned int id = 0;
        unsigned int size = 0;
        double latitude = 0;
        double longitude = 0;

        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<unsigned int>>> infected = {};

        Location() = default;
    };

}

#endif // LOCATION_H
