
#if !defined(DIKE_POINT_HPP)
#define DIKE_POINT_HPP

#include <cstdint>

class DikePoint {
public:
        DikePoint (void);
        DikePoint (double lon, double lat);
        DikePoint (double lon, double lat, double ele);
        DikePoint (double lon, double lat, double ele, double tim);
        ~DikePoint (void);

        double lon (void) const;
        double lat (void) const;
        double ele (void) const;
        double tim (void) const;

        void setLon (double lon);
        void setLat (double lat);
        void setEle (double ele);
        void setTim (double tim);

        static double DikePointDistanceEuclidean (const DikePoint *a, const DikePoint *b);
        static DikePoint DikePointCalculateDerivedPosition (const DikePoint *a, double range, double bearing);

private:
        double _lon;
        double _lat;
        double _ele;
        double _tim;
};

#endif
