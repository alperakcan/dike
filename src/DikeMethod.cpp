
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <string>

#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikeMethodBruteForce.hpp"
#include "DikeMethodQuadTree.hpp"

DikeMethodOptions::DikeMethodOptions (void)
{
        coverageRadius = 250;
}

DikeMethodOptions::~DikeMethodOptions (void)
{

}

DikeMethod::DikeMethod (const DikeMethodOptions &options)
{
        _coverageRadius = options.coverageRadius;
}

DikeMethod::~DikeMethod (void)
{

}

int DikeMethod::addTrack (DikePath &path)
{
        return addTrack(&path);
}

int DikeMethod::addRecord (DikePath &path)
{
        return addRecord(&path);
}

int DikeMethod::getCoverageRadius (void)
{
        return _coverageRadius;
}

DikeMethod * DikeMethod::DikeMethodCreateWithType (DikeMethod::Type type, const DikeMethodOptions &options)
{
        if (type == DikeMethod::TypeBruteForce) {
                return new DikeMethodBruteForce(options);
        }
        if (type == DikeMethod::TypeQuadTree) {
                return new DikeMethodQuadTree(options);
        }
        dikeErrorf("type: %d is invalid", type);
        return NULL;
}

DikeMethod * DikeMethod::DikeMethodCreateWithType (std::string &type, const DikeMethodOptions &options)
{
        return DikeMethodCreateWithType(DikeMethodTypeFromString(type), options);
}

DikeMethod * DikeMethod::DikeMethodCreateWithType (const char *type, const DikeMethodOptions &options)
{
        return DikeMethodCreateWithType(DikeMethodTypeFromString(type), options);
}

DikeMethod::Type DikeMethod::DikeMethodTypeFromString (std::string &type)
{
        return DikeMethodTypeFromString(type.c_str());
}

DikeMethod::Type DikeMethod::DikeMethodTypeFromString (const char *type)
{
        if (type == NULL) {
                return DikeMethod::TypeInvalid;
        }
        if (strcasecmp(type, "bruteforce") == 0) {
                return DikeMethod::TypeBruteForce;
        }
        if (strcasecmp(type, "quadtree") == 0) {
                return DikeMethod::TypeQuadTree;
        }
        return DikeMethod::TypeInvalid;
}
