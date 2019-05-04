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
 *  Copyright 2019, Jan Broeckhove.
 */

#include "WorkplaceCSVReader.h"

#include "util/CSV.h"

namespace geopop {

using namespace std;
using namespace stride::util;

WorkplaceCSVReader::WorkplaceCSVReader(std::unique_ptr<std::istream> inputStream)
    : m_input_stream(std::move(inputStream))
{
}

void WorkplaceCSVReader::SetReferenceWorkplaces(unsigned int&        ref_average_workplace_size,
                                                std::vector<double>& ref_ratios)
{
        CSV reader(*(m_input_stream.get()));
        double         average_size = 0U;
        vector<double> ratios;
        for (const CSVRow& row : reader) {
                std::cout << row << std::endl;
                double       ratio;
                unsigned int min;
                unsigned int max;
                try {
                        ratio = row.GetValue<double>(0);
                        min   = row.GetValue<unsigned int>(1);
                        max   = row.GetValue<unsigned int>(2);
                } catch (const std::bad_cast& e) {
                        // NA
                        break;
                }
                average_size += ratio * (max + min);

                ratios.emplace_back(ratio);
        }
        ref_average_workplace_size = (unsigned int)round(average_size);

        ref_ratios = ratios;
}

} // namespace geopop
