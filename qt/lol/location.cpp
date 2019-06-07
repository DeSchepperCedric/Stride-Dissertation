//
// Created by wannes on 30.04.19.
//

#include "location.h"

using namespace stride;
using namespace geogrid;

location::location(double longtitude, double latitude, int cases) {
    m_latitude = latitude;
    m_longtitude = longtitude;
    m_cases = cases;
}

location::~location() {}