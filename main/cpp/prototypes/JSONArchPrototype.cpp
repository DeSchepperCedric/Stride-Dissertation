//
// Created by stijn on 19/03/19.
//
#include "util/FileSys.h"

#include <boost/lexical_cast.hpp>
#include <tclap/CmdLine.h>
#include <nlohmann/json.hpp>
#include <regex>

// using json = nlohmann::json;
using namespace std;
using namespace stride;
using namespace stride::util;
using namespace TCLAP;

double getDouble(const nlohmann::json& j)
{
        if (j.type() == nlohmann::json::value_t::string) {
                return boost::lexical_cast<double>(j.get<std::string>());
        } else {
                return j.get<double>();
        }
}

/// Testing if the JSON parser of nlohmann can be used.
int main(/*int argc, char** argv*/)
{
        int exitStatus = EXIT_SUCCESS;

        try {
                // CmdLine cmd("JSONproto", ' ', "1.0");

                // string js = "name of the json file";
                // ValueArg<string> s("j", "json", js, false, "holidays_flanders_2016.json", "CONFIGURATION", cmd);

                // cmd.parse(argc, static_cast<const char* const*>(argv));

                // auto config = s.getValue();

                // auto file = FileSys::GetDataDir();
                // file /= config;

                // std::unique_ptr<std::istream> m_inputStream = make_unique<ifstream>(file.string());
                // auto              instream = make_unique<istringstream>("");

                nlohmann::json j;
                //*m_inputStream >> j;

                // SET
                j["commutes"] = {{"1", "0.25"}, {"2", 0.75}};
                j["test"]     = {"1", "2"};
                j["test2"]    = {{1, "2"}, {"1", 2, 3}};
                nlohmann::json testing =
                    R"(
{
	"households_name": "test_household",
	"households_list": [
		[42,38,15],
		[70,68],
		[40,39,9,6],
		[43,42],
		[55,54],
		[40,40,3,3],
		[35,32,6,3],
		[78,75]
	]
}
)"_json;
                nlohmann::json households = testing.at("households_list");

                for (auto household = households.begin(); household != households.end(); household++) {

                        vector<unsigned int> temp;
                        for (auto person = household->begin(); person != household->end(); person++) {
                                std::cout << (*person) << std::endl;
                                unsigned int age;
                                if (person->type() == nlohmann::json::value_t::string) {
                                        age = boost::lexical_cast<unsigned int>(person->get<std::string>());
                                } else {
                                        age = person->get<unsigned int>();
                                }
                                temp.emplace_back(age);
                        }
                }
                std::cout << testing.at("households_name").get<std::string>() << std::endl;
                for (auto household = j["test2"].begin(); household != j["test2"].end(); household++) {

                        vector<unsigned int> temp;
                        for (auto person = household->begin(); person != household->end(); person++) {
                                unsigned int age;
                                if (person->type() == nlohmann::json::value_t::string) {
                                        age = boost::lexical_cast<unsigned int>(person->get<std::string>());
                                } else {
                                        age = person->get<unsigned int>();
                                }
                                temp.emplace_back(age);
                                std::cout << (age) << std::endl;
                        }
                        for (auto it : temp) {
                                std::cout << it << std::endl;
                        }
                }

                // GET
                std::cout << j.at("commutes") << '\n';
                auto commutes = j.at("commutes");
                auto test     = j.at("test");
                for (auto it = test.begin(); it != test.end(); it++) {
                        auto person_id = boost::lexical_cast<unsigned int>(it->get<std::string>());
                        std::cout << person_id << std::endl;
                }
                for (auto it = commutes.begin(); it != commutes.end(); it++) {
                        unsigned int i = boost::lexical_cast<unsigned int>(it.key());
                        double       d = getDouble(*it);
                        // double d = it->get<double>();
                        std::cout << i << " " << d << std::endl;
                }
                // create an array using push_back
                nlohmann::json json;
                json.push_back("foo");
                json.push_back(1);
                json.push_back(true);

                // also use emplace_back
                json.emplace_back(1.78);

                // iterate the array
                for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
                        std::cout << *it << '\n';
                }
                std::cout << j.dump();

                nlohmann::json j1 =
                    R"({"locations":[{"contactCenters":[],"coordinate":{"latitude":0.0,"longitude":0.0},"id":1,"name":"Bavikhove","population":2500,"province":4},{"contactCenters":[],"coordinate":{"latitude":0.0,"longitude":0.0},"id":3,"name":"Mons","population":2500,"province":2},{"contactCenters":[],"coordinate":{"latitude":0.0,"longitude":0.0},"id":2,"name":"Gent","population":5000,"province":3}],"persons":[]})"_json;
                nlohmann::json j2 =
                    R"({"locations":[{"contactCenters":[],"coordinate":{"latitude":0.0,"longitude":0.0},"id":1,"name":"Bavikhove","population":2500,"province":4},{"contactCenters":[],"coordinate":{"latitude":0.0,"longitude":0.0},"id":2,"name":"Gent","population":5000,"province":3},{"contactCenters":[],"coordinate":{"latitude":0.0,"longitude":0.0},"id":3,"name":"Mons","population":2500,"province":2}],"persons":[]})"_json;

                std::cout << std::endl << (j1 == j2) << std::endl;

        } catch (exception& e) {
                exitStatus = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: " << e.what() << endl;
        } catch (...) {
                exitStatus = EXIT_FAILURE;
                cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
        }
        return exitStatus;
}
