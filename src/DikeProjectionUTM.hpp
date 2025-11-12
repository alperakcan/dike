#if !defined(DIKE_PROJECTION_UTM_HPP)
#define DIKE_PROJECTION_UTM_HPP

#include "DikeProjection.hpp"

class DikeProjectionUTM :
        public DikeProjection {
public:
        DikeProjectionUTM (void);
        DikeProjectionUTM (int zone, bool isNorthern);
        virtual ~DikeProjectionUTM (void) override;

        virtual void forward (double lon, double lat, double &x, double &y) override;
        virtual void inverse (double x, double y, double &lon, double &lat) override;

        // Utility functions
        static int getZoneFromLongitude (double lon);
        static bool isNorthernHemisphere (double lat);
        void setZone (int zone, bool isNorthern);
        int getZone (void) const;
        bool getIsNorthern (void) const;

private:
        int _zone;
        bool _isNorthern;
        bool _autoZone;

        // WGS84 ellipsoid parameters
        static constexpr double DIKE_A = 6378137.0;              // Semi-major axis (equatorial radius)
        static constexpr double DIKE_F = 1.0 / 298.257223563;    // Flattening
        static constexpr double DIKE_E2 = DIKE_F * (2.0 - DIKE_F); // First eccentricity squared
        static constexpr double DIKE_K0 = 0.9996;                // Scale factor
        static constexpr double DIKE_E0 = 500000.0;              // False easting
        static constexpr double DIKE_N0_NORTH = 0.0;             // False northing (north)
        static constexpr double DIKE_N0_SOUTH = 10000000.0;      // False northing (south)
};

#endif
