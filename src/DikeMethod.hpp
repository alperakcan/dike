
#if !defined(DIKE_METHOD_HPP)
#define DIKE_METHOD_HPP

#include <tuple>

#include "DikePath.hpp"

class DikeMethod {
public:
        typedef enum : int32_t {
                TypeInvalid,
                TypeBruteForce,
                TypeQuadTree
        } Type;

        typedef enum : int32_t {
                CalculateFieldStatus            = 0,
                CalculateFieldMatchedPoints     = 1,
                CalculateFieldTotalPoints       = 2,
                CalculateFieldMatchedDistance   = 3,
                CalculateFieldTotalDistance     = 4
        } CalculateField;

        DikeMethod (void);
        virtual ~DikeMethod (void);

        virtual int addTrack (DikePath &path);
        virtual int addRecord (DikePath &path);

        virtual int addTrack (DikePath *path) = 0;
        virtual int addRecord (DikePath *path) = 0;

        virtual std::tuple<int, int, int, double, double> calculate (void) = 0;

        static DikeMethod * DikeMethodCreateWithType (DikeMethod::Type type);
        static DikeMethod * DikeMethodCreateWithType (std::string &type);

        static DikeMethod::Type DikeMethodTypeFromString (std::string &type);
};

#endif
