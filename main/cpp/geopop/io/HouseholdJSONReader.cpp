#include "HouseholdJSONReader.h"

#include "util/Exception.h"

namespace geopop {

    using namespace std;
    using namespace stride::util;

    HouseholdJSONReader::HouseholdJSONReader(std::unique_ptr<std::istream> inputStream)
            : m_input_stream(std::move(inputStream))
    {
    }

    void HouseholdJSONReader::SetReferenceHouseholds(unsigned int&                           ref_person_count,
                                                    std::vector<std::vector<unsigned int>>& ref_ages)
    {
        nlohmann::json reader;
        try {
                *m_input_stream >> reader;

        } catch (nlohmann::json::parse_error& e) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        }
        unsigned int p_count = 0U;
        nlohmann::json households = reader.at("households_list");
        auto households_name      = reader.at("households_name").get<std::string>();
        for (auto household = households.begin(); household != households.end(); household++) {

            vector<unsigned int> temp;
            for (auto person = household->begin(); person != household->end(); person++) {
                unsigned int age;
                if (person->type() == nlohmann::json::value_t::string){
                    age = boost::lexical_cast<unsigned int>(person->get<std::string>());
                }else{
                    age = person->get<unsigned int>();
                }
                temp.emplace_back(age);
            }
            p_count += temp.size();
            ref_ages.emplace_back(temp);
        }
        ref_person_count = p_count;
    }

} // namespace geopop
