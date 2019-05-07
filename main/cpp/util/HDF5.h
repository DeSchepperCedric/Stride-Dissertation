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

#include <iostream>

namespace stride {
namespace util {

using namespace std;

struct PERSON
{
        unsigned int id;
        float        age;
        unsigned int k12school;
        unsigned int college;
        unsigned int household;
        unsigned int workplace;
        unsigned int primarycommunity;
        unsigned int secondarycommunity;
        unsigned int daycare;
        unsigned int preschool;
};

struct COMMUTE
{
        unsigned int to;
        double       proportion;
};

struct POOL
{
        unsigned int people;
};

} // namespace util
} // namespace stride
