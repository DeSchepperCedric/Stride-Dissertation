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

#include "HDF5.h"

namespace stride {
namespace util {

using namespace std;
using namespace H5;

CompType GetPersonType()
{
        CompType person_type(sizeof(PersonType));

        person_type.insertMember("Id", HOFFSET(PersonType, id), PredType::NATIVE_UINT);
        person_type.insertMember("Age", HOFFSET(PersonType, age), PredType::NATIVE_FLOAT);
        person_type.insertMember("K12School", HOFFSET(PersonType, k12school), PredType::NATIVE_UINT);
        person_type.insertMember("College", HOFFSET(PersonType, college), PredType::NATIVE_UINT);
        person_type.insertMember("Household", HOFFSET(PersonType, household), PredType::NATIVE_UINT);
        person_type.insertMember("Workplace", HOFFSET(PersonType, workplace), PredType::NATIVE_UINT);
        person_type.insertMember("PrimaryCommunity", HOFFSET(PersonType, primarycommunity), PredType::NATIVE_UINT);
        person_type.insertMember("SecondaryCommunity", HOFFSET(PersonType, secondarycommunity), PredType::NATIVE_UINT);
        person_type.insertMember("Daycare", HOFFSET(PersonType, daycare), PredType::NATIVE_UINT);
        person_type.insertMember("PreSchool", HOFFSET(PersonType, preschool), PredType::NATIVE_UINT);

        return person_type;
}

CompType GetCommuteType()
{
        CompType commute_type(sizeof(CommuteType));

        commute_type.insertMember("To", HOFFSET(CommuteType, to), PredType::NATIVE_UINT);
        commute_type.insertMember("Proportion", HOFFSET(CommuteType, proportion), PredType::NATIVE_DOUBLE);

        return commute_type;
}

CompType GetPoolType()
{
        CompType pool_type(sizeof(PoolType));

        pool_type.insertMember("People", HOFFSET(PoolType, people), PredType::NATIVE_UINT);

        return pool_type;
}

PredType GetPredType(const unsigned int& data)
{
        return PredType::NATIVE_UINT;
}

PredType GetPredType(const unsigned long& data)
{
        return PredType::NATIVE_ULONG;
}

PredType GetPredType(const float& data)
{
        return PredType::NATIVE_FLOAT;
}

PredType GetPredType(const double& data)
{
        return PredType::NATIVE_DOUBLE;
}

StrType GetPredType(const string& data)
{
        return StrType(H5::PredType::C_S1, data.length());
}

} // namespace util
} // namespace stride
