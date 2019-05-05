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

#include "GeoGridConfig.h"

#include "contact/AgeBrackets.h"
#include "contact/ContactType.h"
#include "geopop/io/HouseholdReader.h"
#include "geopop/io/ReaderFactory.h"
#include "geopop/io/WorkplaceReader.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <iomanip>

namespace geopop {

using namespace std;
using namespace boost::property_tree;
using namespace stride::AgeBrackets;
using namespace stride::ContactType;
using stride::util::intToDottedString;

GeoGridConfig::GeoGridConfig() : params{}, refHouseHolds{}, regionsInfo{} {}

GeoGridConfig::GeoGridConfig(const ptree& configPt) : GeoGridConfig()
{
//        const auto pt                      = configPt.get_child("run.geopop_gen");

}

void GeoGridConfig::SetData(const ptree& configPt)
{
//        configPt contains the run.geopop_gen child of the configPt
//        make SetData be able to handle a variable number of regions


//        arrays in boost ptrees:
//        auto array = configPt.get_child("array");
//        for (auto it = array.begin(); it != array.end(); it++) {
//                // it->second.get_child("") is now a ptree from the array
//        }
        std::string workplacesFileName = configPt.get<string>("workplace_file", "");

        //----------------------------------------------------------------
        // Set workplace size distribution values when file present.
        //----------------------------------------------------------------
        if (!workplacesFileName.empty()) {
                std::cout << "READING" <<std::endl;
                auto workplaceReader = ReaderFactory::CreateWorkplaceReader(workplacesFileName);
                workplaceReader->SetReferenceWorkplaces(refWP.average_workplace_size, refWP.ratios);
        }
        else{
                std::cout << "NOT READING" <<std::endl;
        }

        people[Id::Daycare]            = configPt.get<unsigned int>("people_per_Daycare", 18U);
        people[Id::PreSchool]          = configPt.get<unsigned int>("people_per_PreSchool", 120U);
        people[Id::K12School]          = configPt.get<unsigned int>("people_per_K12School", 500U);
        people[Id::College]            = configPt.get<unsigned int>("people_per_College", 3000U);
        people[Id::Workplace]          = configPt.get<unsigned int>("people_per_Workplace", 20U);
        people[Id::PrimaryCommunity]   = configPt.get<unsigned int>("people_per_PrimaryCommunity", 2000U);
        people[Id::SecondaryCommunity] = configPt.get<unsigned int>("people_per_SecondaryCommunity", 2000U);

        //TODO why no pools per preschool/ alternative: for all, but setters with default 1?
        pools[Id::K12School]           = configPt.get<unsigned int>("pools_per_K12School", 25U);
        pools[Id::College]             = configPt.get<unsigned int>("pools_per_College", 20U);

        auto regionArray = configPt.get_child("regions");
        for (auto it = regionArray.begin(); it != regionArray.end(); ++it){
                Param param;
                const auto regionPt                = it->second.get_child("");
                const auto regionId                = regionPt.get<unsigned int>("id",0);

                param.pop_size                     = regionPt.get<unsigned int>("population_size");
                param.participation_college        = regionPt.get<double>("participation_college");
                param.fraction_workplace_commuters = regionPt.get<double>("fraction_workplace_commuters");
                param.fraction_college_commuters   = regionPt.get<double>("fraction_college_commuters");
                param.participation_workplace      = regionPt.get<double>("participation_workplace");
                param.participation_preschool      = regionPt.get<double>("participation_preschool");
                param.participation_daycare        = regionPt.get<double>("participation_daycare");
                params[regionId] = param;

                RefHH refHH;
                Info info;

                const auto householdsReader = ReaderFactory::CreateHouseholdReader(regionPt.get<string>("household_file"));
                householdsReader->SetReferenceHouseholds(refHH.person_count, refHH.ages);
                info = ParseHouseholdInfo(refHH.person_count, refHH.ages, param);

                const auto major_household_file = regionPt.get<string>("major_household_file", "");
                if (!major_household_file.empty()) {
                        const auto majorHouseholdsReader = ReaderFactory::CreateHouseholdReader(major_household_file);
                        majorHouseholdsReader->SetReferenceHouseholds(refHH.major_person_count, refHH.major_ages);

                        //Add the major info to the info struct
                        Info major_info = ParseHouseholdInfo(refHH.major_person_count, refHH.major_ages, param);

                        info.major_popcount_daycare = major_info.popcount_daycare;
                        info.major_popcount_preschool = major_info.popcount_preschool;
                        info.major_popcount_k12school = major_info.popcount_k12school;
                        info.major_popcount_college = major_info.popcount_college;
                        info.major_popcount_workplace = major_info.popcount_workplace;
                        info.major_count_households = major_info.count_households;

                } else {
                        info.major_popcount_daycare = 0;
                        info.major_popcount_preschool = 0;
                        info.major_popcount_k12school = 0;
                        info.major_popcount_college = 0;
                        info.major_popcount_workplace = 0;
                        info.major_count_households = 0;
                        refHH.major_person_count = 0;
                }

                refHouseHolds[regionId] = refHH;
                regionsInfo[regionId] = info;

        }
}

GeoGridConfig::Info GeoGridConfig::ParseHouseholdInfo(unsigned int ref_p_count, std::vector<std::vector<unsigned int>>& ages, Param& param)
{
        const auto popSize = param.pop_size;
        const auto refHHSize = ages.size();

        //----------------------------------------------------------------
        // Determine age makeup of reference household population.
        //----------------------------------------------------------------
        const auto averageHhSize = static_cast<double>(ref_p_count) / static_cast<double>(refHHSize);

        auto ref_daycare   = 0U;
        auto ref_preschool = 0U;
        auto ref_k12school = 0U;
        auto ref_college   = 0U;
        auto ref_workplace = 0U;

        for (const auto& hhAgeProfile : ages) {
                for (const auto& age : hhAgeProfile) {
                        if (Daycare::HasAge(age)) {
                                ref_daycare++;
                        }
                        if (PreSchool::HasAge(age)) {
                                ref_preschool++;
                        }
                        if (K12School::HasAge(age)) {
                                ref_k12school++;
                        }
                        if (College::HasAge(age)) {
                                ref_college++;
                        }
                        if (Workplace::HasAge(age)) {
                                ref_workplace++;
                        }
                }
        }
        //----------------------------------------------------------------
        // Scale up to the generated population size.
        //----------------------------------------------------------------
        const auto fraction_daycare_age   = static_cast<double>(ref_daycare) / static_cast<double>(ref_p_count);
        const auto fraction_preschool_age = static_cast<double>(ref_preschool) / static_cast<double>(ref_p_count);
        const auto fraction_k12school_age = static_cast<double>(ref_k12school) / static_cast<double>(ref_p_count);
        const auto fraction_college_age   = static_cast<double>(ref_college) / static_cast<double>(ref_p_count);
        const auto fraction_workplace_age = static_cast<double>(ref_workplace) / static_cast<double>(ref_p_count);

        const auto age_count_daycare   = static_cast<unsigned int>(floor(popSize * fraction_daycare_age));
        const auto age_count_preschool = static_cast<unsigned int>(floor(popSize * fraction_preschool_age));
        const auto age_count_k12school = static_cast<unsigned int>(floor(popSize * fraction_k12school_age));
        const auto age_count_college   = static_cast<unsigned int>(floor(popSize * fraction_college_age));
        const auto age_count_workplace = static_cast<unsigned int>(floor(popSize * fraction_workplace_age));

        Info info;

        info.popcount_daycare = static_cast<unsigned int>(floor(param.participation_daycare * age_count_daycare));
        info.popcount_preschool = static_cast<unsigned int>(floor(param.participation_preschool * age_count_preschool));
        info.popcount_k12school = age_count_k12school;
        info.popcount_college = static_cast<unsigned int>(floor(param.participation_college * age_count_college));
        info.popcount_workplace = static_cast<unsigned int>(
                floor(param.participation_workplace * (age_count_workplace - info.popcount_college)));
        info.count_households = static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));

        return info;
}


ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int w = 53;
        for (const auto &it : config.params) {
                out.setf(std::ios_base::left, std::ios_base::adjustfield);
                out << "Input:" << endl;
                out << setw(w) << "Fraction college commuters:" << it.second.fraction_college_commuters
                    << "\n";
                out << setw(w) << "Fraction workplace commuters:" << it.second.fraction_workplace_commuters
                    << "\n";
                out << setw(w) << "Participation fraction of daycare:" << it.second.participation_daycare
                    << "\n";
                out << setw(w) << "Participation fraction of preschool:" << it.second.participation_preschool
                    << "\n";
                out << setw(w) << "Participation fraction of college:" << it.second.participation_college
                    << "\n";
                out << setw(w) << "Participation fraction of workplace:" << it.second.participation_workplace
                    << "\n";
                out << setw(w) << "Target population size" << intToDottedString(it.second.pop_size) << "\n"
                    << "\n";
                out << "Calculated:"
                    << "\n";
                out << setw(w) << "Daycare student count:"
                    << intToDottedString(config.regionsInfo.at(it.first).popcount_daycare) << "\n";
                out << setw(w) << "PreSchool student count:"
                    << intToDottedString(config.regionsInfo.at(it.first).popcount_preschool) << "\n";
                out << setw(w) << "K12School student count:"
                    << intToDottedString(config.regionsInfo.at(it.first).popcount_k12school) << "\n";
                out << setw(w) << "College student count:"
                    << intToDottedString(config.regionsInfo.at(it.first).popcount_college) << "\n";
                out << setw(w) << "Workplace person count:"
                    << intToDottedString(config.regionsInfo.at(it.first).popcount_workplace) << "\n";
                out << endl;
        }
        return out;
}

} // namespace geopop
