#pragma once

#include "HouseholdReader.h"

#include <vector>
#include <nlohmann/json.hpp>
#include <boost/lexical_cast.hpp>

namespace stride {

    class ContactPool;
    class Person;

} // namespace stride

namespace geopop {

    class HouseholdCenter;

/**
 * Creates a Reader that retrieves the different Household profiles from a given JSON file.
 */
    class HouseholdJSONReader : public HouseholdReader
    {
    public:
        /// Construct the HouseholdJSONReader with an istream containing the JSON data.
        explicit HouseholdJSONReader(std::unique_ptr<std::istream> inputStream);

        /// Add the locations to the GeoGrid.
        void SetReferenceHouseholds(unsigned int&                           ref_person_count,
                                    std::vector<std::vector<unsigned int>>& ref_ages) override;

    private:
        /// Input stream  connected to input data file.
        std::unique_ptr<std::istream> m_input_stream;
    };

} // namespace geopop
