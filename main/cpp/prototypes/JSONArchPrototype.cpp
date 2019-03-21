//
// Created by stijn on 19/03/19.
//
#include "util/FileSys.h"

#include <boost/property_tree/ptree.hpp>
#include <tclap/CmdLine.h>
#include <regex>
#include <nlohmann/json.hpp>
#include <boost/filesystem/fstream.hpp>
#include <tclap/CmdLine.h>
#include <boost/lexical_cast.hpp>

//using json = nlohmann::json;
using namespace std;
using namespace stride;
using namespace stride::util;
using namespace TCLAP;

double getDouble(const nlohmann::json & j){
    if (j.type() == nlohmann::json::value_t::string) {
        return boost::lexical_cast<double>(j.get<std::string>());
    }else{
        return j.get<double>();
    }
}


/// Testing if the JSON parser of nlohmann can be used.
int main(int argc, char** argv)
{
    int exitStatus = EXIT_SUCCESS;

    try {
        CmdLine cmd("JSONproto", ' ', "1.0");

        string js = "name of the json file";
        ValueArg<string> s("j", "json", js, false, "holidays_flanders_2016.json", "CONFIGURATION", cmd);

        cmd.parse(argc, static_cast<const char* const*>(argv));

        auto config = s.getValue();

        auto file = FileSys::GetDataDir();
        file /= config;

        std::unique_ptr<std::istream> m_inputStream = make_unique<ifstream>(file.string());
        auto              instream = make_unique<istringstream>("");

        nlohmann::json j;
        *instream >> j;

        //SET
        j["commutes"] = {{"1", "0.25"}, {"2", 0.75}};
        j["test"] = {"1", "2"};

        //GET
        std::cout << j.at("commutes") << '\n';
        auto commutes = j.at("commutes");
        auto test = j.at("test");
        for (auto it = test.begin(); it != test.end(); it++){
            auto person_id = boost::lexical_cast<unsigned int>(it->get<std::string>());
            std::cout << person_id << std::endl;
        }
        for (auto it = commutes.begin(); it != commutes.end(); it++) {
            unsigned int i = boost::lexical_cast<unsigned int>(it.key());
            double d = getDouble(*it);
            //double d = it->get<double>();
            std::cout << i
                      << " "
                      << d
                      << std::endl;
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
        
    } catch (exception& e) {
        exitStatus = EXIT_FAILURE;
        cerr << "\nEXCEPION THROWN: " << e.what() << endl;
    } catch (...) {
        exitStatus = EXIT_FAILURE;
        cerr << "\nEXCEPION THROWN: Unknown exception." << endl;
    }
    return exitStatus;
}
