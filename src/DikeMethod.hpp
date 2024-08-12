
#if !defined(DIKE_METHOD_HPP)
#define DIKE_METHOD_HPP

#include "DikePath.hpp"

class DikeMethod {
public:
        typedef enum : int32_t {
                TypeInvalid,
                TypeBrute,
                TypeQuadTree
        } Type;

        DikeMethod (void);
        virtual ~DikeMethod (void);

        virtual int addTrack (DikePath &path);
        virtual int addRecord (DikePath &path);

        virtual int addTrack (DikePath *path) = 0;
        virtual int addRecord (DikePath *path) = 0;

        virtual double calculate (void) = 0;

        static DikeMethod * DikeMethodCreateWithType (DikeMethod::Type type);
        static DikeMethod * DikeMethodCreateWithType (std::string &type);

        static DikeMethod::Type DikeMethodTypeFromString (std::string &type);
};

#endif
