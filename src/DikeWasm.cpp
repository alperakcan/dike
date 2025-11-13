

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <memory>
#include <cstdarg>

#define DIKE_DEBUG_NAME "dike"
#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikePath.hpp"

enum {
        DIKE_INFLATE_FORMAT_INVALID,
        DIKE_INFLATE_FORMAT_KML,
        DIKE_INFLATE_FORMAT_GPX
};

struct DikeInflateResult {
        int32_t length;
        char *buffer;
};

class DikeInflate {
public:
        DikeInflate (void) {
                format = DIKE_INFLATE_FORMAT_INVALID;
                coverageRadius = 0;
        }

        ~DikeInflate (void) {
        }

        int format;
        int coverageRadius;
        std::vector<std::unique_ptr<DikePath>> tpaths;
        std::vector<std::unique_ptr<DikePath>> ipaths;
};

struct DikeCalculateResult {
        int32_t matched_points;
        int32_t total_points;
        double matched_distance;
        double total_distance;
};

struct DikeCalculate {
        DikeMethod *method;
};

extern "C" int inflateDestroy (struct DikeInflate *dikeInflate)
{
        if (dikeInflate == NULL) {
                return 0;
        }
        delete dikeInflate;
        return 0;
}

extern "C" struct DikeInflate * inflateCreate (void)
{
        struct DikeInflate *dikeInflate;

        dikeInflate = new DikeInflate();
        if (dikeInflate == NULL) {
                dikeErrorf("can not allocate memory");
                goto bail;
        }
        return dikeInflate;
bail:   if (dikeInflate != NULL) {
                inflateDestroy(dikeInflate);
        }
        return NULL;
}

extern "C" int inflateReset (struct DikeInflate *dikeInflate)
{
        if (dikeInflate == NULL) {
                dikeErrorf("dikeInflate is invalid");
                goto bail;
        }
        dikeInflate->format = DIKE_INFLATE_FORMAT_INVALID;
        dikeInflate->coverageRadius = 0;
        dikeInflate->tpaths.clear();
        dikeInflate->ipaths.clear();
        return 0;
bail:   return -1;
}

extern "C" int inflateSetOptions (struct DikeInflate *dikeInflate, const char *format, int coverageRadius)
{
        if (dikeInflate == NULL) {
                dikeErrorf("dikeInflate is invalid");
                goto bail;
        }

        if (format == NULL) {
                dikeErrorf("format is invalid");
                goto bail;
        }
        if (strcmp(format, "kml") == 0) {
                dikeInflate->format = DIKE_INFLATE_FORMAT_KML;
        } else if (strcmp(format, "gpx") == 0) {
                dikeInflate->format = DIKE_INFLATE_FORMAT_GPX;
        } else {
                dikeErrorf("format: %s is invalid", format);
                goto bail;
        }

        if (coverageRadius < 0) {
                dikeErrorf("coverageRadius: %d is invalid", coverageRadius);
                goto bail;
        }
        dikeInflate->coverageRadius = coverageRadius;

        return 0;
bail:   return -1;
}

extern "C" int inflateAddTrack (struct DikeInflate *dikeInflate, const char *buffer, int length)
{
        int rc;
        DikePath *path;

        path = NULL;

        if (dikeInflate == NULL) {
                dikeErrorf("dikeInflate is invalid");
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

        path = DikePath::DikePathCreateFromBuffer(buffer, length);
        if (path == NULL) {
                dikeErrorf("can not create path");
                goto bail;
        }
        dikeDebugf("pointsCount: %d", path->getPointsCount());
        dikeInflate->tpaths.push_back(std::unique_ptr<DikePath>(path));

        return 0;
bail:   if (path != NULL) {
                path->decref();
        }
        return -1;
}

extern "C" struct DikeInflateResult * inflateInflate (struct DikeInflate *dikeInflate)
{
        int i;
        int j;
        int jl;
        std::string output;
        struct DikeInflateResult *inflateResult;

        output = "";
        inflateResult = NULL;

        auto appendf = [&](const char *fmt, ...) {
                va_list args;
                va_start(args, fmt);
                char buf[1024];
                int n = vsnprintf(buf, sizeof(buf), fmt, args);
                va_end(args);
                if (n < 0) return;
                if (n < (int)sizeof(buf)) {
                        output.append(buf, n);
                        return;
                }
                std::vector<char> tmp(n + 1);
                va_start(args, fmt);
                vsnprintf(tmp.data(), tmp.size(), fmt, args);
                va_end(args);
                output.append(tmp.data(), n);
        };

        if (dikeInflate == NULL) {
                dikeErrorf("dikeInflate is invalid");
                goto bail;
        }

        for (i = 0; i < (int) dikeInflate->tpaths.size(); i++) {
                DikePath *tpath;
                DikePath *ipath;
                tpath = dikeInflate->tpaths[i].get();
                ipath = DikePath::DikePathCreateInflatedFromPath(tpath, dikeInflate->coverageRadius);
                if (ipath == NULL) {
                        dikeErrorf("can not inflate path: %p", tpath);
                        goto bail;
                }
                dikeDebugf("pointsCount: %d / %d", tpath->getPointsCount(), ipath->getPointsCount());
                dikeInflate->ipaths.push_back(std::unique_ptr<DikePath>(ipath));
        }

        if (dikeInflate->format == DIKE_INFLATE_FORMAT_GPX) {
                appendf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
                appendf("<gpx version=\"1.1\" creator=\"DikeInflate\">\n");
        } else if (dikeInflate->format == DIKE_INFLATE_FORMAT_KML) {
                appendf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
                appendf("<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");

                appendf("<Document>\n");
                appendf("  <name>%s</name>\n", "inflated tracks");
        }

        for (i = 0; i < (int) dikeInflate->ipaths.size(); i++) {
                int m;
                DikePath *ipath;

                ipath = dikeInflate->ipaths[i].get();

                if (dikeInflate->format == DIKE_INFLATE_FORMAT_KML) {
                        appendf("  <Placemark>\n");
                        appendf("    <name>%s (inflated %d meters)</name>\n", "inflated track", dikeInflate->coverageRadius);
                        appendf("    <MultiGeometry>\n");
                }

                for (m = 0, j = 0, jl = ipath->getPointsCount(); j < jl; j++) {
                        std::tuple<DikePath::Command, DikePoint> *point;
                        point = ipath->getPoint(j);
                        if (std::get<0>(*point) == DikePath::CommandMoveTo) {
                                if (dikeInflate->format == DIKE_INFLATE_FORMAT_GPX) {
                                        if (m) {
                                                appendf("    </trkseg>\n");
                                                appendf("  </trk>\n");
                                        }
                                        appendf("  <trk>\n");
                                        appendf("    <name>%s (inflated %d meters)</name>\n", "inflated track", dikeInflate->coverageRadius);
                                        appendf("    <trkseg>\n");
                                } else if (dikeInflate->format == DIKE_INFLATE_FORMAT_KML) {
                                        if (m) {
                                                appendf("        </coordinates>\n");
                                                appendf("      </LineString>\n");

                                        }
                                        appendf("      <LineString>\n");
                                        appendf("        <tessellate>1</tessellate>\n");
                                        appendf("        <coordinates>\n");
                                }
                                m = 1;
                        }
                        if (dikeInflate->format == DIKE_INFLATE_FORMAT_GPX) {
                                appendf("      <trkpt lat=\"%.8f\" lon=\"%.8f\"></trkpt>\n", std::get<1>(*point).lat(), std::get<1>(*point).lon());
                        } else if (dikeInflate->format == DIKE_INFLATE_FORMAT_KML) {
                                appendf("        %.8f,%.8f,0\n", std::get<1>(*point).lon(), std::get<1>(*point).lat());
                        }
                }
                if (dikeInflate->format == DIKE_INFLATE_FORMAT_GPX) {
                        if (m) {
                                appendf("    </trkseg>\n");
                                appendf("  </trk>\n");
                        }
                } else if (dikeInflate->format == DIKE_INFLATE_FORMAT_KML) {
                        if (m) {
                                appendf("        </coordinates>\n");
                                appendf("      </LineString>\n");
                                appendf("    </MultiGeometry>\n");
                                appendf("  </Placemark>\n");
                        }
                }
        }

        if (dikeInflate->format == DIKE_INFLATE_FORMAT_GPX) {
                appendf("</gpx>\n");
        } else if (dikeInflate->format == DIKE_INFLATE_FORMAT_KML) {
                appendf("</Document>\n");
                appendf("</kml>\n");
        }

        inflateResult = (struct DikeInflateResult *) malloc(sizeof(struct DikeInflateResult));
        if (inflateResult == NULL) {
                dikeErrorf("can not allocate memory");
                goto bail;
        }
        inflateResult->length = (int32_t) output.size();
        inflateResult->buffer = (char *) malloc(inflateResult->length);
        if (inflateResult->buffer == NULL) {
                dikeErrorf("can not allocate memory");
                free(inflateResult);
                goto bail;
        }
        memcpy(inflateResult->buffer, output.data(), inflateResult->length);
        dikeErrorf("inflate length: %d", inflateResult->length);

        return inflateResult;
bail:   if (inflateResult != NULL) {
                if (inflateResult->buffer != NULL) {
                        free(inflateResult->buffer);
                }
                free(inflateResult);
        }
        return NULL;
}

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
