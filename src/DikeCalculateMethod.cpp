
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <string>

#include "DikeDebug.hpp"
#include "DikeCalculateMethod.hpp"
#include "DikeCalculateMethodBruteForce.hpp"
#include "DikeCalculateMethodQuadTree.hpp"

DikeCalculateMethodOptions::DikeCalculateMethodOptions (void)
{
        coverageRadius = 250;
}

DikeCalculateMethodOptions::~DikeCalculateMethodOptions (void)
{

}

DikeCalculateMethod::DikeCalculateMethod (const DikeCalculateMethodOptions &options)
{
        _coverageRadius = options.coverageRadius;
}

DikeCalculateMethod::~DikeCalculateMethod (void)
{

}

int DikeCalculateMethod::addTrack (DikePath &path)
{
        return addTrack(&path);
}

int DikeCalculateMethod::addRecord (DikePath &path)
{
        return addRecord(&path);
}

int DikeCalculateMethod::getCoverageRadius (void)
{
        return _coverageRadius;
}

DikeCalculateMethod * DikeCalculateMethod::DikeCalculateMethodCreateWithType (DikeCalculateMethod::Type type, const DikeCalculateMethodOptions &options)
{
        if (type == DikeCalculateMethod::TypeBruteForce) {
                return new DikeCalculateMethodBruteForce(options);
        }
        if (type == DikeCalculateMethod::TypeQuadTree) {
                return new DikeCalculateMethodQuadTree(options);
        }
        dikeErrorf("type: %d is invalid", type);
        return NULL;
}

DikeCalculateMethod * DikeCalculateMethod::DikeCalculateMethodCreateWithType (std::string &type, const DikeCalculateMethodOptions &options)
{
        return DikeCalculateMethodCreateWithType(DikeCalculateMethodTypeFromString(type), options);
}

DikeCalculateMethod * DikeCalculateMethod::DikeCalculateMethodCreateWithType (const char *type, const DikeCalculateMethodOptions &options)
{
        return DikeCalculateMethodCreateWithType(DikeCalculateMethodTypeFromString(type), options);
}

DikeCalculateMethod::Type DikeCalculateMethod::DikeCalculateMethodTypeFromString (std::string &type)
{
        return DikeCalculateMethodTypeFromString(type.c_str());
}

DikeCalculateMethod::Type DikeCalculateMethod::DikeCalculateMethodTypeFromString (const char *type)
{
        if (type == NULL) {
                return DikeCalculateMethod::TypeInvalid;
        }
        if (strcasecmp(type, "bruteforce") == 0) {
                return DikeCalculateMethod::TypeBruteForce;
        }
        if (strcasecmp(type, "quadtree") == 0) {
                return DikeCalculateMethod::TypeQuadTree;
        }
        return DikeCalculateMethod::TypeInvalid;
}
