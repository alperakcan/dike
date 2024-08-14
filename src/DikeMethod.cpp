
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <string>

#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikeMethodBruteForce.hpp"
#include "DikeMethodQuadTree.hpp"

DikeMethod::DikeMethod (void)
{

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

DikeMethod * DikeMethod::DikeMethodCreateWithType (DikeMethod::Type type)
{
        if (type == DikeMethod::TypeBruteForce) {
                return new DikeMethodBruteForce();
        }
        if (type == DikeMethod::TypeQuadTree) {
                return new DikeMethodQuadTree();
        }
        dikeErrorf("type: %d is invalid", type);
        return NULL;
}

DikeMethod * DikeMethod::DikeMethodCreateWithType (std::string &type)
{
        return DikeMethodCreateWithType(DikeMethodTypeFromString(type));
}

DikeMethod * DikeMethod::DikeMethodCreateWithType (const char *type)
{
        return DikeMethodCreateWithType(DikeMethodTypeFromString(type));
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
