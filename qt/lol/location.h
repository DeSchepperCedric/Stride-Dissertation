//
// Created by wannes on 30.04.19.
//

#ifndef STRIDE_LOCATION_H
#define STRIDE_LOCATION_H
namespace stride{
    namespace geogrid{
        class location {
        public:
            location(double longtitude, double latitude, int cases);
            ~location();

        private:
            double m_latitude;
            double m_longtitude;
            int m_cases;
        };

    }
}



#endif //STRIDE_LOCATION_H
