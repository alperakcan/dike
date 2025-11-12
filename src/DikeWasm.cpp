

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIKE_DEBUG_NAME "dike"
#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikePath.hpp"

struct DikeCalculateResult {
        int32_t matched_points;
        int32_t total_points;
        double matched_distance;
        double total_distance;
};

struct DikeCalculate {
        DikeMethod *method;
};

extern "C" int calculateDestroy (struct DikeCalculate *dikeCalculate)
{
        if (dikeCalculate != NULL) {
                if (dikeCalculate->method != NULL) {
                        delete dikeCalculate->method;
                }
                free(dikeCalculate);
        }
        return 0;
}

extern "C" struct DikeCalculate * calculateCreate (void)
{
        struct DikeCalculate *dikeCalculate;

        dikeCalculate = (struct DikeCalculate *) malloc(sizeof(struct DikeCalculate));
        if (dikeCalculate == NULL) {
                dikeErrorf("can not allocate memory");
                goto bail;
        }
        memset(dikeCalculate, 0, sizeof(struct DikeCalculate));
        return dikeCalculate;
bail:   if (dikeCalculate != NULL) {
                calculateDestroy(dikeCalculate);
        }
        return NULL;
}

extern "C" int calculateReset (struct DikeCalculate *dikeCalculate)
{
        if (dikeCalculate == NULL) {
                dikeErrorf("dikeCalculate is invalid");
                goto bail;
        }
        if (dikeCalculate->method != NULL) {
                delete dikeCalculate->method;
                dikeCalculate->method = NULL;
        }
        return 0;
bail:   return -1;
}

extern "C" int calculateSetMethod (struct DikeCalculate *dikeCalculate, const char *method, int coverageRadius)
{
        int rc;
        DikeMethodOptions options;

        if (dikeCalculate == NULL) {
                dikeErrorf("dikeCalculate is invalid");
                goto bail;
        }
        if (method == NULL) {
                dikeErrorf("method is invalid");
                goto bail;
        }

        if (dikeCalculate->method != NULL) {
                delete dikeCalculate->method;
                dikeCalculate->method = NULL;
        }

        if (coverageRadius < 0) {
                dikeErrorf("coverageRadius: %d is invalid", coverageRadius);
                goto bail;
        }

        options = DikeMethodOptions();
        options.coverageRadius = coverageRadius;

        dikeCalculate->method = DikeMethod::DikeMethodCreateWithType(method, options);
        if (dikeCalculate->method == NULL) {
                dikeErrorf("can not create method: %s", method);
                goto bail;
        }

        return 0;
bail:   return -1;
}

extern "C" int calculateAddTrack (struct DikeCalculate *dikeCalculate, const char *buffer, int length)
{
        int rc;
        DikePath *path;

        path = NULL;

        if (dikeCalculate == NULL) {
                dikeErrorf("dikeCalculate is invalid");
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

        if (dikeCalculate->method == NULL) {
                dikeErrorf("dike method is invalid");
                goto bail;
        }

        path = DikePath::DikePathCreateFromBuffer(buffer, length);
        if (path == NULL) {
                dikeErrorf("can not create path");
                goto bail;
        }
        rc = dikeCalculate->method->addTrack(path);
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

extern "C" int calculateAddRecord (struct DikeCalculate *dikeCalculate, const char *buffer, int length)
{
        int rc;
        DikePath *path;

        path = NULL;

        if (dikeCalculate == NULL) {
                dikeErrorf("dikeCalculate is invalid");
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

        if (dikeCalculate->method == NULL) {
                dikeErrorf("dikeCalculate method is invalid");
                goto bail;
        }

        path = DikePath::DikePathCreateFromBuffer(buffer, length);
        if (path == NULL) {
                dikeErrorf("can not create path");
                goto bail;
        }
        rc = dikeCalculate->method->addRecord(path);
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

extern "C" struct DikeCalculateResult * calculateCalculate (struct DikeCalculate *dikeCalculate)
{
        struct DikeCalculateResult *calculateResult;
        std::tuple<int, int, int, double, double> calc;

        if (dikeCalculate == NULL) {
                dikeErrorf("dikeCalculate is invalid");
                goto bail;
        }
        if (dikeCalculate->method == NULL) {
                dikeErrorf("dikeCalculate method is invalid");
                goto bail;
        }

        calc = dikeCalculate->method->calculate();
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


        calculateResult = (struct DikeCalculateResult *) malloc(sizeof(struct DikeCalculateResult));
        calculateResult->matched_points   = std::get<DikeMethod::CalculateFieldMatchedPoints>(calc);
        calculateResult->total_points     = std::get<DikeMethod::CalculateFieldTotalPoints>(calc);
        calculateResult->matched_distance = std::get<DikeMethod::CalculateFieldMatchedDistance>(calc);
        calculateResult->total_distance   = std::get<DikeMethod::CalculateFieldTotalDistance>(calc);

        return calculateResult;
bail:   return NULL;
}
