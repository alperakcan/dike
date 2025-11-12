
#include <cstdlib>
#include "DikeProjection.hpp"
#include "DikeProjectionMercator.hpp"

DikeProjection * DikeProjection::DikeProjectionCreate (DikeProjectionType type)
{
        switch (type) {
                case DIKE_PROJECTION_TYPE_MERCATOR:
                        return new DikeProjectionMercator();
                default:
                        break;
        }
        return NULL;
}

DikeProjection::~DikeProjection (void)
{
}
