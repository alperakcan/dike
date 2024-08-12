
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <string>

#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikeMethodBrute.hpp"
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
        if (type == DikeMethod::TypeBrute) {
                return new DikeMethodBrute();
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

DikeMethod::Type DikeMethod::DikeMethodTypeFromString (std::string &type)
{
        if (strcasecmp(type.c_str(), "brute") == 0) {
                return DikeMethod::TypeBrute;
        }
        if (strcasecmp(type.c_str(), "quadtree") == 0) {
                return DikeMethod::TypeQuadTree;
        }
        return DikeMethod::TypeInvalid;
}
