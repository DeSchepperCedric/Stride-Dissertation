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

#include "WorkplaceReader.h"

#include <vector>

namespace stride {

class ContactPool;
class Person;

} // namespace stride

namespace geopop {

/**
 * Create a Reader that retrieves the different Workplace size distributions from a given CSV file.
 */
class WorkplaceCSVReader : public WorkplaceReader
{
public:
        /// Construct the WorkplaceCSVReader with an istream containing the CSV data.
        explicit WorkplaceCSVReader(std::unique_ptr<std::istream> inputStream);

        /// Add the Workplace size distribution values to geogridConfig variables
        void SetWorkplaceSizeDistributions(std::vector<double>& ref_ratios, std::vector<unsigned int>& ref_min,
                                           std::vector<unsigned int>& ref_max) override;

private:
        /// Input stream  connected to input data file.
        std::unique_ptr<std::istream> m_input_stream;
};

} // namespace geopop
