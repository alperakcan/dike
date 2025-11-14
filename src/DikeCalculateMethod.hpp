
#if !defined(DIKE_METHOD_HPP)
#define DIKE_METHOD_HPP

#include <tuple>
#include <memory>

#include "DikePath.hpp"

class DikeCalculateMethodOptions {
public:
        DikeCalculateMethodOptions (void);
        ~DikeCalculateMethodOptions (void);

        int coverageRadius;
};

class DikeCalculateMethod {
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
                CalculateFieldTotalDistance     = 4,
                CalculateFieldMatchedPath       = 5,
                CalculateFieldRejectedPath      = 6
        } CalculateField;

        DikeCalculateMethod (const DikeCalculateMethodOptions &options);
        virtual ~DikeCalculateMethod (void);

        virtual int addTrack (DikePath &path);
        virtual int addRecord (DikePath &path);

        virtual int addTrack (DikePath *path) = 0;
        virtual int addRecord (DikePath *path) = 0;

        int getCoverageRadius (void);

        virtual std::tuple<
                int,
                int,
                int,
                double,
                double,
                std::unique_ptr<DikePath>,
                std::unique_ptr<DikePath>
                > calculate (void) = 0;

        static DikeCalculateMethod * DikeCalculateMethodCreateWithType (DikeCalculateMethod::Type type, const DikeCalculateMethodOptions &options);
        static DikeCalculateMethod * DikeCalculateMethodCreateWithType (std::string &type, const DikeCalculateMethodOptions &options);
        static DikeCalculateMethod * DikeCalculateMethodCreateWithType (const char *type, const DikeCalculateMethodOptions &options);

        static DikeCalculateMethod::Type DikeCalculateMethodTypeFromString (std::string &type);
        static DikeCalculateMethod::Type DikeCalculateMethodTypeFromString (const char *type);

private:
        int _coverageRadius;
};

#endif
