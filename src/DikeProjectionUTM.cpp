
#include <cmath>
#include "DikeDebug.hpp"
#include "DikeProjectionUTM.hpp"

DikeProjectionUTM::DikeProjectionUTM (void)
{
        _zone = 0;
        _isNorthern = true;
        _autoZone = true;
}

DikeProjectionUTM::DikeProjectionUTM (int zone, bool isNorthern)
{
        _zone = zone;
        _isNorthern = isNorthern;
        _autoZone = false;
}

DikeProjectionUTM::~DikeProjectionUTM (void)
{
}

int DikeProjectionUTM::getZoneFromLongitude (double lon)
{
        // Normalize lon to [-180, 180)
        while (lon < -180.0) lon += 360.0;
        while (lon >= 180.0) lon -= 360.0;

        // UTM zones are 6 degrees wide, numbered 1-60
        // Zone 1 starts at -180° (180°W)
        int zone = static_cast<int>(std::floor((lon + 180.0) / 6.0)) + 1;

        // Clamp to valid range [1, 60]
        if (zone < 1) zone = 1;
        if (zone > 60) zone = 60;

        return zone;
}

int DikeProjectionUTM::getZoneFromLonLat (double lon, double lat)
{
        // Normalize lon to [-180, 180)
        while (lon < -180.0) lon += 360.0;
        while (lon >= 180.0) lon -= 360.0;

        // Base zone by longitude
        int zone = getZoneFromLongitude(lon);

        // Norway exception: lat between 56N and 64N and lon between 3E and 12E -> use zone 32
        if (lat >= 56.0 && lat < 64.0 && lon >= 3.0 && lon < 12.0) {
                zone = 32;
        }

        // Svalbard exceptions: lat between 72N and 84N
        if (lat >= 72.0 && lat < 84.0) {
                if (lon >= 0.0 && lon < 9.0) zone = 31;
                else if (lon >= 9.0 && lon < 21.0) zone = 33;
                else if (lon >= 21.0 && lon < 33.0) zone = 35;
                else if (lon >= 33.0 && lon < 42.0) zone = 37;
        }

        // Clamp to [1,60] (defensive)
        if (zone < 1) zone = 1;
        if (zone > 60) zone = 60;
        return zone;
}

bool DikeProjectionUTM::isNorthernHemisphere (double lat)
{
        return lat >= 0.0;
}

void DikeProjectionUTM::setZone (int zone, bool isNorthern)
{
        _zone = zone;
        _isNorthern = isNorthern;
        _autoZone = false;
}

int DikeProjectionUTM::getZone (void) const
{
        return _zone;
}

bool DikeProjectionUTM::getIsNorthern (void) const
{
        return _isNorthern;
}

void DikeProjectionUTM::forward (double lon, double lat, double &x, double &y)
{
        constexpr double DIKE_PI = 3.14159265358979323846;

        // Auto-detect zone if needed
        if (_autoZone) {
                // Use lon+lat aware rules (Norway/Svalbard exceptions)
                _zone = getZoneFromLonLat(lon, lat);
                _isNorthern = isNorthernHemisphere(lat);
                dikeDebugf("UTM auto-zone: %d%c for lon=%.6f lat=%.6f", _zone, _isNorthern ? 'N' : 'S', lon, lat);
        }

        // Calculate central meridian for this zone
        // Zone 1 is centered at -177°, zone 2 at -171°, etc.
        double lon0 = (_zone - 1) * 6.0 - 180.0 + 3.0;

        // Convert to radians
        double latRad = lat * DIKE_PI / 180.0;
        double lonRad = lon * DIKE_PI / 180.0;
        double lon0Rad = lon0 * DIKE_PI / 180.0;
        double dLon = lonRad - lon0Rad;

        // Pre-calculate some values
        double sinLat = std::sin(latRad);
        double cosLat = std::cos(latRad);
        double tanLat = std::tan(latRad);

        double N = DIKE_A / std::sqrt(1.0 - DIKE_E2 * sinLat * sinLat);
        double T = tanLat * tanLat;
        double C = DIKE_E2 * cosLat * cosLat / (1.0 - DIKE_E2);
        double A = dLon * cosLat;

        // Calculate M (meridional arc)
        double e4 = DIKE_E2 * DIKE_E2;
        double e6 = e4 * DIKE_E2;

        double M = DIKE_A * ((1.0 - DIKE_E2 / 4.0 - 3.0 * e4 / 64.0 - 5.0 * e6 / 256.0) * latRad
                           - (3.0 * DIKE_E2 / 8.0 + 3.0 * e4 / 32.0 + 45.0 * e6 / 1024.0) * std::sin(2.0 * latRad)
                           + (15.0 * e4 / 256.0 + 45.0 * e6 / 1024.0) * std::sin(4.0 * latRad)
                           - (35.0 * e6 / 3072.0) * std::sin(6.0 * latRad));

        // Calculate easting and northing
        double A2 = A * A;
        double A3 = A2 * A;
        double A4 = A3 * A;
        double A5 = A4 * A;
        double A6 = A5 * A;

        x = DIKE_E0 + DIKE_K0 * N * (A + (1.0 - T + C) * A3 / 6.0
                                     + (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * DIKE_E2 / (1.0 - DIKE_E2)) * A5 / 120.0);

        y = DIKE_K0 * (M + N * tanLat * (A2 / 2.0 + (5.0 - T + 9.0 * C + 4.0 * C * C) * A4 / 24.0
                                         + (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * DIKE_E2 / (1.0 - DIKE_E2)) * A6 / 720.0));

        // Add false northing for southern hemisphere
        if (!_isNorthern) {
                y += DIKE_N0_SOUTH;
        }
}

void DikeProjectionUTM::inverse (double x, double y, double &lon, double &lat)
{
        constexpr double DIKE_PI = 3.14159265358979323846;

        // Remove false easting
        double x1 = x - DIKE_E0;

        // Remove false northing if southern hemisphere
        double y1 = y;
        if (!_isNorthern) {
                y1 -= DIKE_N0_SOUTH;
        }

        // Calculate footpoint latitude
        double M = y1 / DIKE_K0;
        double mu = M / (DIKE_A * (1.0 - DIKE_E2 / 4.0 - 3.0 * DIKE_E2 * DIKE_E2 / 64.0
                                   - 5.0 * DIKE_E2 * DIKE_E2 * DIKE_E2 / 256.0));

        double e1 = (1.0 - std::sqrt(1.0 - DIKE_E2)) / (1.0 + std::sqrt(1.0 - DIKE_E2));

        double phi1 = mu + (3.0 * e1 / 2.0 - 27.0 * e1 * e1 * e1 / 32.0) * std::sin(2.0 * mu)
                         + (21.0 * e1 * e1 / 16.0 - 55.0 * e1 * e1 * e1 * e1 / 32.0) * std::sin(4.0 * mu)
                         + (151.0 * e1 * e1 * e1 / 96.0) * std::sin(6.0 * mu)
                         + (1097.0 * e1 * e1 * e1 * e1 / 512.0) * std::sin(8.0 * mu);

        double sinPhi1 = std::sin(phi1);
        double cosPhi1 = std::cos(phi1);
        double tanPhi1 = std::tan(phi1);

        double N1 = DIKE_A / std::sqrt(1.0 - DIKE_E2 * sinPhi1 * sinPhi1);
        double T1 = tanPhi1 * tanPhi1;
        double C1 = DIKE_E2 * cosPhi1 * cosPhi1 / (1.0 - DIKE_E2);
        double R1 = DIKE_A * (1.0 - DIKE_E2) / std::pow(1.0 - DIKE_E2 * sinPhi1 * sinPhi1, 1.5);
        double D = x1 / (N1 * DIKE_K0);

        double D2 = D * D;
        double D3 = D2 * D;
        double D4 = D3 * D;
        double D5 = D4 * D;
        double D6 = D5 * D;

        // Calculate latitude
        lat = phi1 - (N1 * tanPhi1 / R1) * (D2 / 2.0
                                            - (5.0 + 3.0 * T1 + 10.0 * C1 - 4.0 * C1 * C1 - 9.0 * DIKE_E2 / (1.0 - DIKE_E2)) * D4 / 24.0
                                            + (61.0 + 90.0 * T1 + 298.0 * C1 + 45.0 * T1 * T1 - 252.0 * DIKE_E2 / (1.0 - DIKE_E2) - 3.0 * C1 * C1) * D6 / 720.0);

        // Calculate longitude (lon0 must be in radians here)
        double lon0_deg = (_zone - 1) * 6.0 - 180.0 + 3.0;
        double lon0_rad = lon0_deg * (DIKE_PI / 180.0);
        lon = lon0_rad + (D - (1.0 + 2.0 * T1 + C1) * D3 / 6.0
                      + (5.0 - 2.0 * C1 + 28.0 * T1 - 3.0 * C1 * C1 + 8.0 * DIKE_E2 / (1.0 - DIKE_E2) + 24.0 * T1 * T1) * D5 / 120.0) / cosPhi1;

        // Convert from radians to degrees
        lat = lat * 180.0 / DIKE_PI;
        lon = lon * 180.0 / DIKE_PI;
        // Normalize longitude to [-180, 180)
        while (lon < -180.0) lon += 360.0;
        while (lon >= 180.0) lon -= 360.0;
}
