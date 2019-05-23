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

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;
using namespace H5;

GeoGridHDF5Reader::GeoGridHDF5Reader(const string& inputFile, Population* pop) : GeoGridFileReader(inputFile, pop) {}

void GeoGridHDF5Reader::Read()
{
        H5File file;
        try {
                H5::Exception::dontPrint();

                file = H5File(m_inputFile, H5F_ACC_RDONLY);

        auto& geoGrid = m_population->RefGeoGrid();
        auto  people  = file.openDataSet("persons");

        const int person_dataset_size = people.getStorageSize();
        PERSON    person_data[person_dataset_size];
        people.read(&person_data, person_type);
        for (auto prsn : person_data) {
                auto person               = ParsePerson(prsn);
                m_people[person->GetId()] = person;
        }

        auto         locations      = file.openGroup("locations");
        Attribute    locations_size = locations.openAttribute("size");
        unsigned int size;
        locations_size.read(locations_size.getDataType(), &size);
        const string name = "location";
        for (unsigned int i = 1; i <= size; i++) {
                string location_name    = name + to_string(i);
                auto location           = locations.openGroup(location_name);
                auto loc                = ParseLocation(location);
                geoGrid.addLocation(move(loc.first),loc.second );
        }

                AddCommutes(geoGrid);
                m_commutes.clear();
                m_people.clear();
                file.close();
        } catch (FileIException& error) {
                throw util::Exception(error.getDetailMsg());
        } catch (DataSetIException& error) {
                throw util::Exception(error.getDetailMsg());
        } catch (GroupIException& error) {
                throw util::Exception(error.getDetailMsg());
        } catch (runtime_error&) {
                throw util::Exception("Problem parsing HDF5 file, check whether empty or invalid HDF5.");
        }
}

void GeoGridHDF5Reader::ParseContactPool(H5::DataSet& contactPool, const shared_ptr<Location>& result)
{
        auto   size = ReadAttribute<unsigned int>("size", contactPool);
        string type = ReadAttribute<string>("type", contactPool);

        ContactType::Id typeId;
        if (type == ToString(Id::K12School)) {
                typeId = Id::K12School;
        } else if (type == ToString(Id::College)) {
                typeId = Id::College;
        } else if (type == ToString(Id::Household)) {
                typeId = Id::Household;
        } else if (type == ToString(Id::PrimaryCommunity)) {
                typeId = Id::PrimaryCommunity;
        } else if (type == ToString(Id::SecondaryCommunity)) {
                typeId = Id::SecondaryCommunity;
        } else if (type == ToString(Id::Workplace)) {
                typeId = Id::Workplace;
        } else if (type == ToString(Id::Daycare)) {
                typeId = Id::Daycare;
        } else if (type == ToString(Id::PreSchool)) {
                typeId = Id::PreSchool;
        } else {
                throw util::Exception("No such ContactPool type: " + type);
        }

        if (size != 0) {
                // Don't use the id of the ContactPool but the let the Population create an id.
                auto cp = m_population->RefPoolSys().CreateContactPool(typeId);
                result->RefPools(typeId).emplace_back(cp);
                vector<PoolType> pool_data(size);
                contactPool.read(pool_data.data(), GetPoolType());
                for (auto pool : pool_data) {
                        unsigned int people_id = pool.people;
                        try {
                                const auto person = m_people.at(people_id);
                                cp->AddMember(person);
                                person->SetPoolId(typeId, result->GetID());
                        } catch (out_of_range& e) {
                                throw util::Exception("No such person with id: " + to_string(people_id));
                        }
                }

                result->RegisterPool(cp, typeId);
        }
}

std::pair<std::shared_ptr<Location>, std::shared_ptr<EnhancedCoordinate>> GeoGridHDF5Reader::ParseLocation(H5::Group& location)
{
        auto      id         = ReadAttribute<unsigned int>("id", location);
        string    name       = ReadAttribute<string>("name", location);
        auto      prov       = ReadAttribute<unsigned int>("province", location);
        auto      pop        = ReadAttribute<unsigned int>("population", location);
        Attribute attr_coord = location.openAttribute("coordinates");
        double    coord[2];
        attr_coord.read(attr_coord.getDataType(), &coord);
        Coordinate c      = {boost::lexical_cast<double>(coord[0]), boost::lexical_cast<double>(coord[1])};
        auto       result = make_shared<Location>(id, prov, c, name, pop);

        auto         contactPools = location.openGroup("contactPools");
        auto         pool_size    = ReadAttribute<unsigned int>("size", contactPools);
        const string pool_name    = "pool";
        for (unsigned int i = 1; i <= pool_size; i++) {
                auto pool = contactPools.openDataSet((pool_name + to_string(i)));
                ParseContactPool(pool, result);
        }

        auto                commutes     = location.openDataSet("commutes");
        auto                commute_size = ReadAttribute<unsigned int>("size", commutes);
        vector<CommuteType> commutes_data(commute_size);
        commutes.read(commutes_data.data(), GetCommuteType());
        for (auto cmmt : commutes_data) {
                m_commutes.emplace_back(id, cmmt.to, cmmt.proportion);
        }

        return make_pair(result, make_shared<EnhancedCoordinate>(result.get(), c));
}

Person* GeoGridHDF5Reader::ParsePerson(PersonType& person)
{
        return m_population->CreatePerson(person.id, person.age, person.household, person.k12school, person.college,
                                          person.workplace, person.primarycommunity, person.secondarycommunity,
                                          person.daycare, person.preschool);
}

template <typename T>
T GeoGridHDF5Reader::ReadAttribute(const string& name, H5Object& object)
{
        T         data;
        Attribute attribute = object.openAttribute(name);
        attribute.read(attribute.getDataType(), &data);
        return data;
}

template <>
string GeoGridHDF5Reader::ReadAttribute(const string& name, H5Object& object)
{
        string    data;
        Attribute attribute = object.openAttribute(name);
        attribute.read(attribute.getStrType(), data);
        return data;
}

} // namespace geopop
