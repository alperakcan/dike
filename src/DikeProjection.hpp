
#if !defined(DIKE_PROJECTION_HPP)
#define DIKE_PROJECTION_HPP

class DikeProjection {
public:
        enum DikeProjectionType {
                DIKE_PROJECTION_TYPE_MERCATOR,
                DIKE_PROJECTION_TYPE_LAMBERT,
                DIKE_PROJECTION_TYPE_UNKNOWN
        };

        static DikeProjection *DikeProjectionCreate (DikeProjectionType type);
        virtual ~DikeProjection (void);

        virtual void forward (double lon, double lat, double &x, double &y) = 0;
        virtual void inverse (double x, double y, double &lon, double &lat) = 0;
};

#endif
