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

#include "H5Cpp.h"

#include <iostream>

namespace stride {
namespace util {

struct PersonType
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

struct CommuteType
{
        unsigned int to;
        double       proportion;
};

struct PoolType
{
        unsigned int people;
};

H5::CompType GetPersonType();

H5::CompType GetCommuteType();

H5::CompType GetPoolType();

template <typename T>
H5::PredType GetPredType()
{
        if (std::is_same<T, unsigned int>::value) {
                return H5::PredType::NATIVE_UINT;
        }
        if (std::is_same<T, unsigned long>::value) {
                return H5::PredType::NATIVE_ULONG;
        }
        if (std::is_same<T, float>::value) {
                return H5::PredType::NATIVE_FLOAT;
        }
        if (std::is_same<T, double>::value) {
                return H5::PredType::NATIVE_DOUBLE;
        }
}

/*
H5::PredType  GetPredType(const unsigned int& data);

H5::PredType  GetPredType(const unsigned long& data);

H5::PredType  GetPredType(const float& data);

H5::PredType  GetPredType(const double& data);
*/
H5::StrType GetPredType(const std::string& data);

} // namespace util
} // namespace stride
