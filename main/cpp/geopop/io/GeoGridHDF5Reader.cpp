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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "GeoGridHDF5Reader.h"

#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <memory>
#include <sstream>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

GeoGridHDF5Reader::GeoGridHDF5Reader(unique_ptr<istream> inputStream, Population* pop)
    : GeoGridReader(move(inputStream), pop), strdatatype(H5::PredType::C_S1, 256), person_type(sizeof(PERSON)), commute_type(sizeof(COMMUTE)), pool_type(sizeof(POOL))
{
        person_type.insertMember("Id", HOFFSET(PERSON, id), PredType::NATIVE_UINT);
        person_type.insertMember("Age", HOFFSET(PERSON, age), PredType::NATIVE_FLOAT);
        person_type.insertMember("K12School", HOFFSET(PERSON, k12school), PredType::NATIVE_UINT);
        person_type.insertMember("College", HOFFSET(PERSON, college), PredType::NATIVE_UINT);
        person_type.insertMember("Household", HOFFSET(PERSON, household), PredType::NATIVE_UINT);
        person_type.insertMember("Workplace", HOFFSET(PERSON, workplace), PredType::NATIVE_UINT);
        person_type.insertMember("PrimaryCommunity", HOFFSET(PERSON, primarycommunity), PredType::NATIVE_UINT);
        person_type.insertMember("SecondaryCommunity", HOFFSET(PERSON, secondarycommunity), PredType::NATIVE_UINT);
        person_type.insertMember("Daycare", HOFFSET(PERSON, daycare), PredType::NATIVE_UINT);
        person_type.insertMember("PreSchool", HOFFSET(PERSON, preschool), PredType::NATIVE_UINT);

        commute_type.insertMember("To", HOFFSET(COMMUTE, to), PredType::NATIVE_UINT);
        commute_type.insertMember("Proportion", HOFFSET(COMMUTE, proportion), PredType::NATIVE_DOUBLE);

        pool_type.insertMember("People", HOFFSET(POOL, people), PredType::NATIVE_UINT);
}

void GeoGridHDF5Reader::Read()
{
        H5File file;
        try {
                ostringstream ss;
                ss << (*m_inputStream).rdbuf();
                file = H5File(ss.str(), H5F_ACC_RDONLY);
        } catch (FileIException error) {
                throw error.getDetailMsg();
        } /*catch (runtime_error&) {
                throw Exception("Problem parsing HDF5 file, check whether empty or invalid HDF5.");
        }*/

        auto& geoGrid = m_population->RefGeoGrid();
        auto people = file.openDataSet("persons");

        PERSON person_data[people.getStorageSize()];
        people.read(&person_data, person_type);
        for (auto prsn : person_data) {
                auto person               = ParsePerson(prsn);
                m_people[person->GetId()] = person;
        }

        auto locations = file.openGroup("locations");
        Attribute locations_size = locations.openAttribute("size");
        unsigned int size;
        locations_size.read(locations_size.getDataType(), &size);
        const string name = "location";
        for (auto i = 1; i <= size; i++) {
                string location_name = name + to_string(i);
                shared_ptr<Location> loc;
                auto location = locations.openGroup(location_name);
                loc = ParseLocation(location);
                geoGrid.AddLocation(move(loc));
        }

        AddCommutes(geoGrid);
        m_commutes.clear();
        m_people.clear();
}

shared_ptr<Location> GeoGridHDF5Reader::ParseLocation(Group& location)
{
        Attribute attr_id     = location.openAttribute("id");
        Attribute attr_name   = location.openAttribute("name");
        Attribute attr_prov   = location.openAttribute("province");
        Attribute attr_pop    = location.openAttribute("population");
        Attribute attr_coord  = location.openAttribute("coordination");
        unsigned int  id;
        string        name;
        unsigned int  prov;
        unsigned int  pop;
        double        coord[2];
        attr_id.read(attr_id.getDataType(), &id);
        attr_name.read(attr_id.getDataType(), &name);
        attr_prov.read(attr_id.getDataType(), &prov);
        attr_pop.read(attr_id.getDataType(), &pop);
        attr_coord.read(attr_id.getDataType(), &coord);
        Coordinate c = {boost::lexical_cast<double>(coord[0]), boost::lexical_cast<double>(coord[1])};
        auto result = make_shared<Location>(id, prov, c, name, pop);

        return result;
}

Person* GeoGridHDF5Reader::ParsePerson(PERSON& person)
{
        return m_population->CreatePerson(
                                            person.id,
                                            person.age,
                                            person.household,
                                            person.k12school,
                                            person.college,
                                            person.workplace,
                                            person.primarycommunity,
                                            person.secondarycommunity,
                                            person.daycare,
                                            person.preschool
                                          );
}

} // namespace geopop
