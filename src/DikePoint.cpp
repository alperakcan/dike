
#include "DikePoint.h"

DikePoint::DikePoint (void)
        : DikePoint(INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN)
{

}

DikePoint::DikePoint (int32_t lat, int32_t lon)
        : DikePoint(lat, lon, INT32_MIN, INT32_MIN)
{

}

DikePoint::DikePoint (int32_t lat, int32_t lon, int32_t ele)
        : DikePoint(lat, lon, ele, INT32_MIN)
{

}

DikePoint::DikePoint (int32_t lat, int32_t lon, int32_t ele, int32_t tim)
{
        _lat = lat;
        _lon = lon;
        _ele = ele;
        _tim = tim;
}

DikePoint::~DikePoint (void)
{

}

int32_t DikePoint::lat (void) const
{
        return _lat;
}

int32_t DikePoint::lon (void) const
{
        return _lon;
}

int32_t DikePoint::ele (void) const
{
        return _ele;
}

int32_t DikePoint::tim (void) const
{
        return _tim;
}

void DikePoint::setLat (int32_t lat)
{
        _lat = lat;
}

void DikePoint::setLon (int32_t lon)
{
        _lon = lon;
}

void DikePoint::setEle (int32_t ele)
{
        _ele = ele;
}

void DikePoint::setTim (int32_t tim)
{
        _tim = tim;
}
