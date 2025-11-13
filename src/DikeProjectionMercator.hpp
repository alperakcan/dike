
#if !defined(DIKE_PROJECTION_MERCATOR_HPP)
#define DIKE_PROJECTION_MERCATOR_HPP

#include "DikeProjection.hpp"

class DikeProjectionMercator :
        public DikeProjection {
public:
        DikeProjectionMercator (void);
        virtual ~DikeProjectionMercator (void) override;

        virtual void forward (double lon, double lat, double &x, double &y) override;
        virtual void inverse (double x, double y, double &lon, double &lat) override;
};

#endif
