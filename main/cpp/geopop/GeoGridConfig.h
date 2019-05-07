#pragma once
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

#include "contact/ContactPool.h"
#include "contact/IdSubscriptArray.h"
#include "pop/Person.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree_fwd.hpp>

#include <map>
#include <memory>
#include <ostream>

namespace geopop {

class GeoGrid;

struct WorkplaceType
{
        /// Minimum size of Workplace type.
        int size_min;

        /// Maximum size of Workplace type.
        int size_max;

        /// Ratio of Workplace type.
        double ratio;
};

/**
 * Configuration data mostly for generating a population, but also for computing
 * the required number of schools, workplaces, communities etc. for that population.
 */
class GeoGridConfig
{
public:
        /// Default constructor needed in test code.
        GeoGridConfig();

        /// Constructor that configures input data.
        explicit GeoGridConfig(const boost::property_tree::ptree& configPt);

        /// People per unit (= Household, K12School, College, etc.) for each of the ContactTypes.
        /// Default initialization. Order in which contacttypes are listed in the
        /// definition of the enumeration must be respected!
        stride::ContactType::IdSubscriptArray<unsigned int> people{0U, 18U, 120U, 500U, 3000U, 20U, 2000U, 2000U};

        /// Pools per unit (= Household, K12School, College, etc.) for each of the ContactTypes.
        /// Default initialization. Order in which contacttypes are listed in the
        /// definition of the enumeration must be respected!
        stride::ContactType::IdSubscriptArray<unsigned int> pools{1U, 1U, 6U, 25U, 20U, 1U, 1U, 1U};

        // -----------------------------------------------------------------------------------------
        // Parameters set by constructor with configuration property tree.
        // -----------------------------------------------------------------------------------------
        struct Param
        {
                /// Participation of daycare (fraction of people of daycare age going to daycare).
                double participation_daycare;

                /// Participation of preschool (fraction of people of preschool age going to preschool).
                double participation_preschool;

                /// Participation of college (fraction of people of college age going to college).
                double participation_college;

                /// Participation of workplace (fraction of people of work age and not going to
                /// college and having employment).
                double participation_workplace;

                /// Fraction of college students that commute.
                double fraction_college_commuters;

                /// Fraction of people in the workplace that commute.
                double fraction_workplace_commuters;

                /// Target population size for the generated population.
                unsigned int pop_size;
        };

        std::map<unsigned int, Param> params;

        // -----------------------------------------------------------------------------------------
        // The reference Households used to generate the population by random draws.
        // -----------------------------------------------------------------------------------------
        struct RefHH
        {
                /// Number of persons in the reference household set.
                unsigned int person_count = 0U;

                /// Age profile per reference household.
                std::vector<std::vector<unsigned int>> ages{};

                /// Number of persons in the reference household set for central cities.
                unsigned int major_person_count = 0U;

                /// Age profile per reference household of central cities.
                std::vector<std::vector<unsigned int>> major_ages{};
        };

        std::map<unsigned int, RefHH> refHouseHolds;

        // -----------------------------------------------------------------------------------------
        // The reference Workplace types used to generate/populate workplaces
        // -----------------------------------------------------------------------------------------
        struct
        {
                /// Average number of persons in a workplace.
                unsigned int average_workplace_size = 0U;

                /// Ratio per workplace type
                std::vector<double> ratios;
        } refWP;

        // -----------------------------------------------------------------------------------------
        // These are numbers derived from the reference households, the target size of the generated
        // population and the input parameters relating participation in college and workplace.
        // These numbers are used as targets in the population generation process and are reproduced
        // (to very close approximation) in the generated population.
        // The numbers are set by the SetData method.
        // -----------------------------------------------------------------------------------------
        struct Info
        {
                /// Numbers of individuals in Daycare.
                unsigned int popcount_daycare;

                /// Numbers of individuals in PreSchool.
                unsigned int popcount_preschool;

                /// Numbers of individuals in K12School.
                unsigned int popcount_k12school;

                /// Number of individuals in College.
                unsigned int popcount_college;

                /// Number of individuals in Workplace.
                unsigned int popcount_workplace;

                /// The number of households.
                unsigned int count_households;

                /// Numbers of individuals in Daycare.
                unsigned int major_popcount_daycare;

                /// Numbers of individuals in PreSchool.
                unsigned int major_popcount_preschool;

                /// Numbers of individuals in K12School.
                unsigned int major_popcount_k12school;

                /// Number of individuals in College.
                unsigned int major_popcount_college;

                /// Number of individuals in Workplace.
                unsigned int major_popcount_workplace;

                /// The number of households.
                unsigned int major_count_households;
        };

        std::map<unsigned int, Info> regionsInfo;

        // -----------------------------------------------------------------------------------------
        /// Read the househould data file, parse it and set data.
        // -----------------------------------------------------------------------------------------
        void SetData(const boost::property_tree::ptree& configPt);

        Info ParseHouseholdInfo(unsigned int ref_p_count, std::vector<std::vector<unsigned int>>& ages, Param& param);
};

} // namespace geopop
