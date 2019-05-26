//
// Created by wannes on 23.04.19.
//

#include "uiModel.h"


using namespace stride;

void uiModel::loadGeoGrid(std::string filename) {

    std::filebuf fb;
    if (fb.open(filename, std::ios::in)) {
        auto is =  unique_ptr<std::istream>(new std::istream(&fb));
        geopop::GeoGridJSONReader(is, this->m_pop)
        fb.close();
    }


}
