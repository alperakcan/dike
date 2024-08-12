
#if !defined(DIKE_PATH_HPP)
#define DIKE_PATH_HPP

#include <string>
#include <tuple>

#include "DikePoint.hpp"

class DikePathPrivate;

class DikePath {
public:
        typedef enum : int32_t {
                CommandInvalid,
                CommandMoveTo,
                CommandLineTo
        } Command;

        DikePath (void);
        virtual ~DikePath (void);

        DikePath * incref (void);
        void decref (void);

        int moveTo (double lon, double lat);
        int moveTo (double lon, double lat, double ele);
        int moveTo (double lon, double lat, double ele, double tim);
        int moveTo (DikePoint &point);

        int lineTo (double lon, double lat);
        int lineTo (double lon, double lat, double ele);
        int lineTo (double lon, double lat, double ele, double tim);
        int lineTo (DikePoint &point);

        int getPointsCount (void);
        std::tuple<DikePath::Command, DikePoint> * getPoint (int index);

        static DikePath * DikePathCreateFromFile (std::string &file);

private:
        int32_t _refcount;
        DikePathPrivate *_private;
};

#endif
