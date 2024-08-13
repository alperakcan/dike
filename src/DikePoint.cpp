
#include <stdio.h>
#include <math.h>

#include "DikePoint.hpp"

DikePoint::DikePoint (void)
        : DikePoint(INFINITY, INFINITY, INFINITY, INFINITY)
{

}

DikePoint::DikePoint (double lon, double lat)
        : DikePoint(lon, lat, INFINITY, INFINITY)
{

}

DikePoint::DikePoint (double lon, double lat, double ele)
        : DikePoint(lon, lat, ele, INFINITY)
{

}

DikePoint::DikePoint (double lon, double lat, double ele, double tim)
{
        _lon = lon;
        _lat = lat;
        _ele = ele;
        _tim = tim;
}

DikePoint::~DikePoint (void)
{

}

double DikePoint::lon (void) const
{
        return _lon;
}

double DikePoint::lat (void) const
{
        return _lat;
}

double DikePoint::ele (void) const
{
        return _ele;
}

double DikePoint::tim (void) const
{
        return _tim;
}

void DikePoint::setLon (double lon)
{
        _lon = lon;
}

void DikePoint::setLat (double lat)
{
        _lat = lat;
}

void DikePoint::setEle (double ele)
{
        _ele = ele;
}

void DikePoint::setTim (double tim)
{
        _tim = tim;
}

double DikePoint::DikePointDistanceEuclidean (const DikePoint *a, const DikePoint *b)
{
        double earthRadius = 6378137.0;
        double dLat = (b->lat() - a->lat()) * M_PI / 180.0;
        double dLng = (b->lon() - a->lon()) * M_PI / 180.0;
        double sindLat = sin(dLat / 2);
        double sindLng = sin(dLng / 2);
        double _a = pow(sindLat, 2) + pow(sindLng, 2) * cos((a->lat() * M_PI / 180.0)) * cos((b->lat() * M_PI / 180.0));
        double _c = 2 * atan2(sqrt(_a), sqrt(1 - _a));
        double dist = earthRadius * _c;
        if (a->ele() != INFINITY && b->ele() != INFINITY) {
                dist = sqrt(pow(a->ele() - b->ele(), 2) + pow(dist, 2));
        }
        return dist;
}

DikePoint DikePoint::DikePointCalculateDerivedPosition (const DikePoint *a, double range, double bearing)
{
        double degreesToRadians = M_PI / 180.0;
        double radiansToDegrees = 180.0 / M_PI;
        double earthRadius = 6378137.0;

        double latA = a->lat() * degreesToRadians;
        double lonA = a->lon() * degreesToRadians;
        double angularDistance = range / earthRadius;
        double trueCourse = bearing * degreesToRadians;

        double lat = asin(
            sin(latA) * cos(angularDistance) +
            cos(latA) * sin(angularDistance) * cos(trueCourse));

        double dlon = atan2(
            sin(trueCourse) * sin(angularDistance) * cos(latA),
            cos(angularDistance) - sin(latA) * sin(lat));

        double lon = fmod(lonA + dlon + M_PI, M_PI * 2) - M_PI;

        return DikePoint(
                lon * radiansToDegrees,
                lat * radiansToDegrees,
                a->ele(),
                a->tim());
}
