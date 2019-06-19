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

#include "EpiReaderFactory.h"

#include "EpiHDF5Reader.h"
#include "EpiJSONReader.h"
#include "EpiReader.h"
#include "util/Exception.h"
#include "util/FileSys.h"

#include <fstream>

#ifdef BOOST_FOUND
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace filesys = boost::filesystem;
#else
#include <filesystem>
namespace filesys = std::filesystem;
#endif

using namespace stride::util;

namespace geopop {

std::shared_ptr<EpiReader> EpiReaderFactory::CreateReader(const std::string& filename) const
{
        //        put "data/" in config file before filename, otherwise smoke import test needs to be updated
        //        const filesys::path path(FileSys::GetDataDir() / filesys::path(filename));
        const filesys::path path(filename);

        if (!filesys::exists(path)) {
                throw stride::util::Exception("EpiReaderFactory::CreateReader> File not found: " + path.string());
        }

        if (path.extension().string() == ".json") {
                auto file = std::make_unique<std::ifstream>();
                file->open(path.string());
                return std::make_shared<EpiJSONReader>(move(file));
        } /* else if (path.extension().string() == ".proto") {
             return std::make_shared<GeoGridProtoReader>(std::make_unique<std::ifstream>(path.string()), pop);
         } */
        else if (path.extension().string() == ".h5") {
                return std::make_shared<EpiHDF5Reader>(path.string());
        } else {
                throw stride::util::Exception("EpiReaderFactory::CreateReader> Unsupported file extension: " +
                                              path.extension().string());
        }
}

} // namespace geopop
