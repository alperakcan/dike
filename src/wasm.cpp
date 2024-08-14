

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIKE_DEBUG_NAME "dike"
#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikePath.hpp"

struct dike {
        DikeMethod *method;
};

extern "C" int destroy (struct dike *dike)
{
        if (dike != NULL) {
                if (dike->method != NULL) {
                        delete dike->method;
                }
                free(dike);
        }
        return 0;
}

extern "C" struct dike * create (void)
{
        struct dike *dike;

        dike = (struct dike *) malloc(sizeof(struct dike));
        if (dike == NULL) {
                dikeErrorf("can not allocate memory");
                goto bail;
        }
        memset(dike, 0, sizeof(struct dike));

        return dike;
bail:   if (dike != NULL) {
                destroy(dike);
        }
        return NULL;
}

extern "C" int reset (struct dike *dike)
{
        if (dike == NULL) {
                dikeErrorf("dike is invalid");
                goto bail;
        }
        if (dike->method != NULL) {
                delete dike->method;
                dike->method = NULL;
        }
        return 0;
bail:   return -1;
}

extern "C" int setMethod (struct dike *dike, const char *method)
{
        int rc;

        if (dike == NULL) {
                dikeErrorf("dike is invalid");
                goto bail;
        }
        if (method == NULL) {
                dikeErrorf("method is invalid");
                goto bail;
        }

        if (dike->method != NULL) {
                delete dike->method;
                dike->method = NULL;
        }

        dike->method = DikeMethod::DikeMethodCreateWithType(method);
        if (dike->method == NULL) {
                dikeErrorf("can not create method: %s", method);
                goto bail;
        }

        return 0;
bail:   return -1;
}

extern "C" int addTrack (struct dike *dike, const char *buffer, int length)
{
        int rc;
        DikePath *path;

        path = NULL;

        if (dike == NULL) {
                dikeErrorf("dike is invalid");
                goto bail;
        }
        if (buffer == NULL) {
                dikeErrorf("buffer is invalid");
                goto bail;
        }
        if (length <= 0) {
                dikeErrorf("length is invalid");
                goto bail;
        }

        if (dike->method == NULL) {
                dikeErrorf("dike method is invalid");
                goto bail;
        }

        path = DikePath::DikePathCreateFromBuffer(buffer, length);
        if (path == NULL) {
                dikeErrorf("can not create path");
                goto bail;
        }
        rc = dike->method->addTrack(path);
        if (rc < 0) {
                dikeErrorf("can not add buffer");
                goto bail;
        }
        path->decref();

        return 0;
bail:   if (path != NULL) {
                path->decref();
        }
        return -1;
}

extern "C" int addRecord (struct dike *dike, const char *buffer, int length)
{
        int rc;
        DikePath *path;

        path = NULL;

        if (dike == NULL) {
                dikeErrorf("dike is invalid");
                goto bail;
        }
        if (buffer == NULL) {
                dikeErrorf("buffer is invalid");
                goto bail;
        }
        if (length <= 0) {
                dikeErrorf("length is invalid");
                goto bail;
        }

        if (dike->method == NULL) {
                dikeErrorf("dike method is invalid");
                goto bail;
        }

        path = DikePath::DikePathCreateFromBuffer(buffer, length);
        if (path == NULL) {
                dikeErrorf("can not create path");
                goto bail;
        }
        rc = dike->method->addRecord(path);
        if (rc < 0) {
                dikeErrorf("can not add buffer");
                goto bail;
        }
        path->decref();

        return 0;
bail:   if (path != NULL) {
                path->decref();
        }
        return -1;
}

extern "C" int calculate (struct dike *dike)
{
        std::tuple<int, int, int, double, double> calc;

        if (dike == NULL) {
                dikeErrorf("dike is invalid");
                goto bail;
        }
        if (dike->method == NULL) {
                dikeErrorf("dike method is invalid");
                goto bail;
        }

        calc = dike->method->calculate();
        if (std::get<DikeMethod::CalculateFieldStatus>(calc) < 0) {
                dikeErrorf("can not calculate");
                goto bail;
        }

        dikeInfof("  points  : %%%.3f (%d / %d)",
                std::get<DikeMethod::CalculateFieldMatchedPoints>(calc) * 100.0 / std::get<DikeMethod::CalculateFieldTotalPoints>(calc),
                std::get<DikeMethod::CalculateFieldMatchedPoints>(calc),
                std::get<DikeMethod::CalculateFieldTotalPoints>(calc));
        dikeInfof("  distance: %%%.3f (%.3f / %.3f)",
                std::get<DikeMethod::CalculateFieldMatchedDistance>(calc) * 100.0 / std::get<DikeMethod::CalculateFieldTotalDistance>(calc),
                std::get<DikeMethod::CalculateFieldMatchedDistance>(calc),
                std::get<DikeMethod::CalculateFieldTotalDistance>(calc));

        return 0;
bail:   return -1;
}
