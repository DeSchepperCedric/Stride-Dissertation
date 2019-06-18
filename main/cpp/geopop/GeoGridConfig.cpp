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

GeoGridConfig::GeoGridConfig() : params{}, refHouseHolds{}, refWP{}, regionsInfo{} {}

GeoGridConfig::GeoGridConfig(const ptree& configPt) : GeoGridConfig()
{
        people[Id::Daycare]            = configPt.get<unsigned int>("people_per_Daycare", 18U);
        people[Id::PreSchool]          = configPt.get<unsigned int>("people_per_PreSchool", 120U);
        people[Id::K12School]          = configPt.get<unsigned int>("people_per_K12School", 500U);
        people[Id::College]            = configPt.get<unsigned int>("people_per_College", 3000U);
        people[Id::Workplace]          = configPt.get<unsigned int>("people_per_Workplace", 20U);
        people[Id::PrimaryCommunity]   = configPt.get<unsigned int>("people_per_PrimaryCommunity", 2000U);
        people[Id::SecondaryCommunity] = configPt.get<unsigned int>("people_per_SecondaryCommunity", 2000U);

        pools[Id::Daycare]            = configPt.get<unsigned int>("pools_per_Daycare", 1U);
        pools[Id::PreSchool]          = configPt.get<unsigned int>("pools_per_PreSchool", 6U);
        pools[Id::K12School]          = configPt.get<unsigned int>("pools_per_K12School", 25U);
        pools[Id::College]            = configPt.get<unsigned int>("pools_per_College", 20U);
        pools[Id::Workplace]          = configPt.get<unsigned int>("pools_per_Workplace", 1U);
        pools[Id::PrimaryCommunity]   = configPt.get<unsigned int>("pools_per_PrimaryCommunity", 1U);
        pools[Id::SecondaryCommunity] = configPt.get<unsigned int>("pools_per_SecondaryCommunity", 1U);
}

void GeoGridConfig::SetData(const ptree& configPt)
{
        //        configPt contains the run.geopop_gen child of the configPt
        //        make SetData be able to handle a variable number of regions

        std::string workplacesFileName = configPt.get<string>("workplace_file", "");

        //----------------------------------------------------------------
        // Set workplace size distribution values when file present.
        //----------------------------------------------------------------
        if (!workplacesFileName.empty()) {
                auto workplaceReader = ReaderFactory::CreateWorkplaceReader(workplacesFileName);
                workplaceReader->SetWorkplaceSizeDistributions(refWP.ratios, refWP.min, refWP.max);

                //----------------------------------------------------------------
                // Distribution performance: fast or accurate
                //----------------------------------------------------------------
                std::string performance = configPt.get<string>("workplace_performance", "");
                if(performance == "fast"){
                    refWP.accurate = false;
                }

        }

        //------------------------------------------------------------------------------------------
        // Set region parameters and info, plus provide a default. region ID of default shall be -1
        //------------------------------------------------------------------------------------------
        // Default
        Param param{};

        param.pop_size                     = configPt.get<unsigned int>("population_size", 600000);
        param.fraction_workplace_commuters = configPt.get<double>("fraction_workplace_commuters", 0.5);
        param.fraction_college_commuters   = configPt.get<double>("fraction_college_commuters", 0.5);
        param.participation_workplace      = configPt.get<double>("participation_workplace", 0.75);
        param.participation_college        = configPt.get<double>("participation_college", 0.5);
        param.participation_preschool      = configPt.get<double>("participation_preschool", 0.99);
        param.participation_daycare        = configPt.get<double>("participation_daycare", 0.45);
        params[-1]                         = param;
        RefHH      refHH{};
        Info       info{};
        const auto household_file = configPt.get<string>("household_file", "");
        if (!household_file.empty()) {
                const auto householdsReader = ReaderFactory::CreateHouseholdReader(household_file);
                householdsReader->SetReferenceHouseholds(refHH.person_count, refHH.ages);
                info = ParseHouseholdInfo(refHH.person_count, refHH.ages, param);
        }

        const auto major_household_file = configPt.get<string>("major_household_file", "");
        if (!major_household_file.empty()) {
                const auto majorHouseholdsReader = ReaderFactory::CreateHouseholdReader(major_household_file);
                majorHouseholdsReader->SetReferenceHouseholds(refHH.major_person_count, refHH.major_ages);

                // Add the major info to the info struct
                Info major_info = ParseHouseholdInfo(refHH.major_person_count, refHH.major_ages, param);

                info.major_fraction_daycare   = major_info.fraction_daycare;
                info.major_fraction_preschool = major_info.fraction_preschool;
                info.major_fraction_k12school = major_info.fraction_k12school;
                info.major_fraction_college   = major_info.fraction_college;
                info.major_fraction_workplace = major_info.fraction_workplace;
                info.major_count_households   = major_info.count_households;

        } else {
                info.major_fraction_daycare   = 0;
                info.major_fraction_preschool = 0;
                info.major_fraction_k12school = 0;
                info.major_fraction_college   = 0;
                info.major_fraction_workplace = 0;
                info.major_count_households   = 0;
                refHH.major_person_count      = 0;
        }

        refHouseHolds[-1] = refHH;
        regionsInfo[-1]   = info;

        // Regions
        if (configPt.count("regions")) {
                auto regionArray = configPt.get_child("regions");
                for (auto it = regionArray.begin(); it != regionArray.end(); ++it) {
                        Param      param{};
                        const auto regionPt = it->second.get_child("");
                        const auto regionId = regionPt.get<unsigned int>("id", 0);

                        param.pop_size = regionPt.get<unsigned int>("population_size", params[-1].pop_size);
                        param.participation_college =
                            regionPt.get<double>("participation_college", params[-1].participation_college);
                        param.fraction_workplace_commuters = regionPt.get<double>(
                            "fraction_workplace_commuters", params[-1].fraction_workplace_commuters);
                        param.fraction_college_commuters =
                            regionPt.get<double>("fraction_college_commuters", params[-1].fraction_college_commuters);
                        param.participation_workplace =
                            regionPt.get<double>("participation_workplace", params[-1].participation_workplace);
                        param.participation_preschool =
                            regionPt.get<double>("participation_preschool", params[-1].participation_preschool);
                        param.participation_daycare =
                            regionPt.get<double>("participation_daycare", params[-1].participation_daycare);
                        params[regionId] = param;

                        RefHH refHH{};
                        Info  info{};

                        const auto household_file = regionPt.get<string>("household_file", "");
                        if (!household_file.empty()) {
                                const auto householdsReader = ReaderFactory::CreateHouseholdReader(household_file);
                                householdsReader->SetReferenceHouseholds(refHH.person_count, refHH.ages);
                                info = ParseHouseholdInfo(refHH.person_count, refHH.ages, param);
                        } else {
                                info  = regionsInfo[-1];
                                refHH = refHouseHolds[-1];
                        }

                        const auto major_household_file = regionPt.get<string>("major_household_file", "");
                        if (!major_household_file.empty()) {
                                const auto majorHouseholdsReader =
                                    ReaderFactory::CreateHouseholdReader(major_household_file);
                                majorHouseholdsReader->SetReferenceHouseholds(refHH.major_person_count,
                                                                              refHH.major_ages);

                                // Add the major info to the info struct
                                Info major_info = ParseHouseholdInfo(refHH.major_person_count, refHH.major_ages, param);

                                info.major_fraction_daycare   = major_info.fraction_daycare;
                                info.major_fraction_preschool = major_info.fraction_preschool;
                                info.major_fraction_k12school = major_info.fraction_k12school;
                                info.major_fraction_college   = major_info.fraction_college;
                                info.major_fraction_workplace = major_info.fraction_workplace;
                                info.major_count_households   = major_info.count_households;
                        }

                        refHouseHolds[regionId] = refHH;
                        regionsInfo[regionId]   = info;
                }
        }
}

GeoGridConfig::Info GeoGridConfig::ParseHouseholdInfo(unsigned int                            ref_p_count,
                                                      std::vector<std::vector<unsigned int>>& ages, Param& param)
{
        const auto popSize   = param.pop_size;
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

        //        const auto age_count_daycare   = static_cast<unsigned int>(floor(popSize * fraction_daycare_age));
        //        const auto age_count_preschool = static_cast<unsigned int>(floor(popSize * fraction_preschool_age));
        //        const auto age_count_k12school = static_cast<unsigned int>(floor(popSize * fraction_k12school_age));
        //        const auto age_count_college   = static_cast<unsigned int>(floor(popSize * fraction_college_age));
        //        const auto age_count_workplace = static_cast<unsigned int>(floor(popSize * fraction_workplace_age));

        Info info;

        info.fraction_daycare   = param.participation_daycare * fraction_daycare_age;
        info.fraction_preschool = param.participation_preschool * fraction_preschool_age;
        info.fraction_k12school = fraction_k12school_age; // Mandatory, so participation == 1
        info.fraction_college   = param.participation_college * fraction_college_age;
        info.fraction_workplace = param.participation_workplace * (fraction_workplace_age - info.fraction_college);
        info.count_households   = static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));

        return info;
}

ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int w = 53;
        for (const auto& it : config.params) {
                out.setf(std::ios_base::left, std::ios_base::adjustfield);
                out << "Input:" << endl;
                out << setw(w) << "Fraction college commuters:" << it.second.fraction_college_commuters << "\n";
                out << setw(w) << "Fraction workplace commuters:" << it.second.fraction_workplace_commuters << "\n";
                out << setw(w) << "Participation fraction of daycare:" << it.second.participation_daycare << "\n";
                out << setw(w) << "Participation fraction of preschool:" << it.second.participation_preschool << "\n";
                out << setw(w) << "Participation fraction of college:" << it.second.participation_college << "\n";
                out << setw(w) << "Participation fraction of workplace:" << it.second.participation_workplace << "\n";
                out << setw(w) << "Target population size" << intToDottedString(it.second.pop_size) << "\n"
                    << "\n";
                out << "Calculated:"
                    << "\n";
                out << setw(w) << "Daycare student fraction:"
                    << intToDottedString(config.regionsInfo.at(it.first).fraction_daycare) << "\n";
                out << setw(w) << "PreSchool student fraction:"
                    << intToDottedString(config.regionsInfo.at(it.first).fraction_preschool) << "\n";
                out << setw(w) << "K12School student fraction:"
                    << intToDottedString(config.regionsInfo.at(it.first).fraction_k12school) << "\n";
                out << setw(w) << "College student fraction:"
                    << intToDottedString(config.regionsInfo.at(it.first).fraction_college) << "\n";
                out << setw(w) << "Workplace person count:"
                    << intToDottedString(config.regionsInfo.at(it.first).fraction_workplace) << "\n";
                out << endl;
        }
        return out;
}

} // namespace geopop
