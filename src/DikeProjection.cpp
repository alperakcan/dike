
#include <cstdlib>
#include "DikeProjection.hpp"
#include "DikeProjectionMercator.hpp"
#include "DikeProjectionUTM.hpp"

DikeProjection * DikeProjection::DikeProjectionCreate (DikeProjectionType type)
{
        switch (type) {
                case DIKE_PROJECTION_TYPE_MERCATOR:
                        return new DikeProjectionMercator();
                case DIKE_PROJECTION_TYPE_UTM:
                        return new DikeProjectionUTM();
                default:
                        break;
        }
        return NULL;
}

DikeProjection::~DikeProjection (void)
{
}
