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

//using json = nlohmann::json;
using namespace std;
using namespace stride;
using namespace stride::util;
using namespace TCLAP;


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


        boost::filesystem::ifstream i(file);
        nlohmann::json j;
        i >> j;

        //SET
        j["pi"] = 3.141;

        //GET
        std::cout << j.at("pi") << '\n';

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
