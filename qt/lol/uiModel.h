//
// Created by wannes on 23.04.19.
//

#ifndef STRIDE_UIMODEL_H
#define STRIDE_UIMODEL_H


#include "../main/cpp/geopop/io/GeoGridReader.h"

namespace stride{
    class uiModel {
    public:
        uiModel();
        ~uiModel();

        void loadGeoGrid(std::string filename);

    private:
        stride::Population* m_pop;


    };
}



#endif //STRIDE_UIMODEL_H
