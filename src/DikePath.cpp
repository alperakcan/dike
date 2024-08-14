
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <math.h>

#include <string>
#include <vector>
#include <tuple>

#include <zip/zip.h>
#include <expat.h>

#if defined(__WINDOWS__)
#include "strptime.h"
#endif

#define DIKE_DEBUG_NAME "path"
#include "DikeDebug.hpp"
#include "DikePoint.hpp"
#include "DikePath.hpp"

static int stringEndsWith (const char *string, const char *end)
{
        int lstring;
        int lend;

        if (string == NULL) {
                goto bail;
        }
        if (end == NULL) {
                goto bail;
        }

        lstring = (int) strlen(string);
        lend    = (int) strlen(end);

        if (lend > lstring) {
                return 0;
        }
        if (strncasecmp(string + lstring - lend, end, lend) == 0) {
                return 1;
        }
        return 0;
bail:   return -1;
}

struct zipOnExtractParam {
        char *data;
        int size;
};

static size_t zipOnExtract (void *arg, uint64_t offset, const void *data, size_t size)
{
        (void) offset;

        struct zipOnExtractParam *param = (struct zipOnExtractParam *) arg;
        param->data = (char *) realloc(param->data, param->size + size + 1);

        memcpy(&(param->data[param->size]), data, size);
        param->size += size;
        param->data[param->size] = 0;

        return size;
}

struct xmlParserKmlParam {
        int lineString;
        int coordinates;
        char *data;
        int size;
        DikePath *path;
};

static void XMLCALL xmlParserKmlStartElement (void *userData, const XML_Char *name, const XML_Char **atts)
{
        struct xmlParserKmlParam *param = (xmlParserKmlParam *) userData;

        (void) param;
        (void) atts;

        dikeTracef("name: %s", name);

        if (strcasecmp(name, "LineString") == 0) {
                param->lineString = 1;
        }
        if (strcasecmp(name, "coordinates") == 0) {
                param->coordinates = 1;
        }
}

static void XMLCALL xmlParserKmlEndElement (void *userData, const XML_Char *name)
{
        struct xmlParserKmlParam *param = (xmlParserKmlParam *) userData;

        (void) param;
        (void) name;

        dikeTracef("name: /%s", name);

        if (strcasecmp(name, "LineString") == 0) {
                param->lineString = 0;
        }
        if (strcasecmp(name, "coordinates") == 0) {
                int i1;
                char *token1;
                char *saveptr1;

                int i2;
                char *token2;
                char *saveptr2;

                int rc;
                double lon;
                double lat;
                double alt;

                dikeTracef("param->size: %d", param->size);

                for (i1 = 0, saveptr1 = NULL, token1 = strtok_r(param->data, " \t\n\r", &saveptr1); token1; i1++, token1 = strtok_r(NULL, " \t\n\r", &saveptr1)) {
                        dikeTracef("  - %s", token1);

                        lon = INFINITY;
                        lat = INFINITY;
                        alt = INFINITY;

                        for (i2 = 0, saveptr2 = NULL, token2 = strtok_r(token1, ",", &saveptr2); token2; i2++, token2 = strtok_r(NULL, ",", &saveptr2)) {
                                dikeTracef("    - %s", token2);
                                if (i2 == 0) lon = strtod(token2, NULL);
                                else if (i2 == 1) lat = strtod(token2, NULL);
                                else if ((i2 == 2) && (strcmp(token2, "0") != 0)) alt = strtod(token2, NULL);
                        }
                        dikeDebugf("  lon: %.7f, lat: %.7f, alt: %.2f", lon, lat, alt);

                        if (i1 == 0) {
                                rc = param->path->moveTo(lon, lat, alt);
                        } else {
                                rc = param->path->lineTo(lon, lat, alt);
                        }
                        if (rc < 0) {
                                dikeErrorf("can not create path");
                        }
                }

                if (param->data != NULL) {
                        free(param->data);
                        param->data = NULL;
                        param->size = 0;
                }

                param->coordinates = 0;
        }
}

static void XMLCALL xmlParserKmlCharacterData (void *userData, const XML_Char *str, int len)
{
        struct xmlParserKmlParam *param = (xmlParserKmlParam *) userData;

        (void) str;
        (void) len;

        if (param->lineString && param->coordinates) {
                dikeTracef("len: %d", len);
                dikeTracef("str: %.*s\n", len, str);

                param->data = (char *) realloc(param->data, param->size + len + 1);
                memcpy(&(param->data[param->size]), str, len);
                param->size += len;
                param->data[param->size] = 0;
        }
}

struct xmlParserGpxParam {
        int trkseg;
        int trkpt;
        int trkpts;
        int ele;
        int time;

        char *data;
        int size;

        double plon;
        double plat;
        double pele;
        double ptim;

        DikePath *path;
};

static void XMLCALL xmlParserGpxStartElement (void *userData, const XML_Char *name, const XML_Char **atts)
{
        struct xmlParserGpxParam *param = (xmlParserGpxParam *) userData;

        (void) param;
        (void) atts;

        dikeTracef("name: %s", name);

        if (strcasecmp(name, "trkseg") == 0) {
                param->trkseg = 1;
        }
        if (strcasecmp(name, "trkpt") == 0) {
                int i;

                param->plon = INFINITY;
                param->plat = INFINITY;
                param->pele = INFINITY;
                param->ptim = INFINITY;

                for (i = 0; atts && atts[i]; i += 2) {
                        dikeTracef("  - %s = %s", atts[i], atts[i + 1]);
                        if (strcasecmp(atts[i], "lon") == 0) {
                                param->plon = atof(atts[i + 1]);
                        } else if (strcasecmp(atts[i], "lat") == 0) {
                                param->plat = atof(atts[i + 1]);
                        }
                }

                param->trkpt = 1;
                param->trkpts += 1;
        }
        if (strcasecmp(name, "ele") == 0) {
                param->ele = 1;
        }
        if (strcasecmp(name, "time") == 0) {
                param->time = 1;
        }
}

static void XMLCALL xmlParserGpxEndElement (void *userData, const XML_Char *name)
{
        struct xmlParserGpxParam *param = (xmlParserGpxParam *) userData;

        (void) param;
        (void) name;

        dikeTracef("name: /%s", name);

        if (strcasecmp(name, "trkseg") == 0) {
                param->trkseg = 0;
        }
        if (strcasecmp(name, "trkpt") == 0) {
                int rc;

                dikeTracef("  lon: %.7f, lat: %.7f, ele: %.2f, tim: %.3f", param->plon, param->plat, param->pele, param->ptim);

                if (param->trkpts == 1) {
                        rc = param->path->moveTo(param->plon, param->plat, param->pele, param->ptim);
                } else {
                        rc = param->path->lineTo(param->plon, param->plat, param->pele, param->ptim);
                }
                if (rc < 0) {
                        dikeErrorf("can not create path");
                }

                param->trkpt = 0;
        }
        if (strcasecmp(name, "ele") == 0) {
                dikeTracef("param->data: %p, size: %d, str: %s", param->data, param->size, param->data);
                if (param->trkseg && param->trkpt && param->data) {
                        param->pele = atof(param->data);
                }
                if (param->data != NULL) {
                        free(param->data);
                        param->data = NULL;
                        param->size = 0;
                }

                param->ele = 0;
        }
        if (strcasecmp(name, "time") == 0) {
                dikeTracef("param->data: %p, size: %d, str: %s", param->data, param->size, param->data);
                if (param->trkseg && param->trkpt && param->data) {
                        struct tm tm;
                        memset(&tm, 0, sizeof(tm));
                        strptime(param->data, "%Y-%m-%dT%H:%M:%S.", &tm);
                        dikeTracef("%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday + 1, tm.tm_hour, tm.tm_min, tm.tm_sec);
                        param->ptim = mktime(&tm);
                }
                if (param->data != NULL) {
                        free(param->data);
                        param->data = NULL;
                        param->size = 0;
                }

                param->time = 0;
        }
}

static void XMLCALL xmlParserGpxCharacterData (void *userData, const XML_Char *str, int len)
{
        struct xmlParserGpxParam *param = (xmlParserGpxParam *) userData;

        (void) str;
        (void) len;


        if (param->trkseg && param->trkpt && param->ele) {
                dikeTracef("len: %d", len);
                dikeTracef("str: %.*s\n", len, str);

                param->data = (char *) realloc(param->data, param->size + len + 1);
                memcpy(&(param->data[param->size]), str, len);
                param->size += len;
                param->data[param->size] = 0;
        }
        if (param->trkseg && param->trkpt && param->time) {
                dikeTracef("len: %d", len);
                dikeTracef("str: %.*s\n", len, str);

                param->data = (char *) realloc(param->data, param->size + len + 1);
                memcpy(&(param->data[param->size]), str, len);
                param->size += len;
                param->data[param->size] = 0;
        }
}

class DikePathPrivate {
public:
        DikePathPrivate (void);
        ~DikePathPrivate (void);

        int moveTo (DikePoint &point);
        int lineTo (DikePoint &point);

        int getPointsCount (void);
        std::tuple<DikePath::Command, DikePoint> * getPoint (int index);

private:
        std::vector<std::tuple<DikePath::Command, DikePoint>> _points;
};

DikePathPrivate::DikePathPrivate (void)
{

}

DikePathPrivate::~DikePathPrivate (void)
{

}

int DikePathPrivate::moveTo (DikePoint &point)
{
        _points.push_back(std::tuple<DikePath::Command, DikePoint>(DikePath::CommandMoveTo, point));
        return 0;
}

int DikePathPrivate::lineTo (DikePoint &point)
{
        _points.push_back(std::tuple<DikePath::Command, DikePoint>(DikePath::CommandLineTo, point));
        return 0;
}

int DikePathPrivate::getPointsCount (void)
{
        return _points.size();
}

std::tuple<DikePath::Command, DikePoint> * DikePathPrivate::getPoint (int index)
{
        return &_points[index];
}

DikePath::DikePath (void)
{
        _refcount = 1;
        _private  = new DikePathPrivate();
}

DikePath::~DikePath (void)
{
        if (_private != NULL) {
                delete _private;
        }
}

DikePath * DikePath::incref (void)
{
        _refcount++;
        return this;
}

void DikePath::decref (void)
{
        _refcount--;
        if (_refcount <= 0) {
                delete this;
        }
}

int DikePath::moveTo (double lon, double lat)
{
        DikePoint pnt(lon, lat);
        return moveTo(pnt);
}

int DikePath::moveTo (double lon, double lat, double ele)
{
        DikePoint pnt(lon, lat, ele);
        return moveTo(pnt);
}

int DikePath::moveTo (double lon, double lat, double ele, double tim)
{
        DikePoint pnt(lon, lat, ele, tim);
        return moveTo(pnt);
}

int DikePath::moveTo (DikePoint &point)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->moveTo(point);
bail:   return -1;
}

int DikePath::lineTo (double lon, double lat)
{
        DikePoint pnt(lon, lat);
        return lineTo(pnt);
}

int DikePath::lineTo (double lon, double lat, double ele)
{
        DikePoint pnt(lon, lat, ele);
        return lineTo(pnt);
}

int DikePath::lineTo (double lon, double lat, double ele, double tim)
{
        DikePoint pnt(lon, lat, ele, tim);
        return lineTo(pnt);
}

int DikePath::lineTo (DikePoint &point)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->lineTo(point);
bail:   return -1;
}

int DikePath::getPointsCount (void)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->getPointsCount();
bail:   return -1;
}

std::tuple<DikePath::Command, DikePoint> * DikePath::getPoint (int index)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->getPoint(index);
bail:   return NULL;
}

DikePath * DikePath::DikePathCreateFromFile (std::string &file)
{
        dikeDebugf("parsing file: %s", file.c_str());
        if (stringEndsWith(file.c_str(), ".kmz") == 1) {
                int rc;

                struct zip_t *zip;
                struct zipOnExtractParam zipParam;

                XML_Parser xmlParser;
                xmlParserKmlParam xmlParam;

                zip = NULL;
                memset(&zipParam, 0, sizeof(zipParam));

                xmlParser = NULL;
                memset(&xmlParam, 0, sizeof(xmlParam));

                zip = zip_open(file.c_str(), 0, 'r');
                if (zip == NULL) {
                        dikeErrorf("can not open zip: %s", file.c_str());
                        goto kmz_bail;
                }
                rc = zip_entry_open(zip, "doc.kml");
                if (rc != 0) {
                        dikeErrorf("can not open zip entry: doc.kml");
                        goto kmz_bail;
                }
                zipParam.data = NULL;
                zipParam.size = 0;
                rc = zip_entry_extract(zip, zipOnExtract, &zipParam);
                if (rc != 0) {
                        dikeErrorf("can not extract zip entry: doc.kml");
                        goto kmz_bail;
                }

                xmlParam.path = new DikePath();
                if (xmlParam.path == NULL) {
                        dikeErrorf("can not create path for file: %s", file.c_str());
                        goto kmz_bail;
                }

                xmlParser = XML_ParserCreate(NULL);
                if (xmlParser == NULL) {
                        dikeErrorf("can not create parser");
                        goto kmz_bail;
                }
                XML_SetUserData(xmlParser, &xmlParam);
                XML_SetElementHandler(xmlParser, xmlParserKmlStartElement, xmlParserKmlEndElement);
                XML_SetCharacterDataHandler(xmlParser, xmlParserKmlCharacterData);

                rc = XML_Parse(xmlParser, zipParam.data, zipParam.size, 1);
                if (rc != XML_STATUS_OK) {
                        dikeErrorf("can not parse file: %s, line: %ld, error: %s", file.c_str(), XML_GetCurrentLineNumber(xmlParser), XML_ErrorString(XML_GetErrorCode(xmlParser)));
                        goto kmz_bail;
                }

                free(xmlParam.data);
                XML_ParserFree(xmlParser);
                free(zipParam.data);
                zip_entry_close(zip);
                zip_close(zip);
                return xmlParam.path;
kmz_bail:
                if (zip != NULL) {
                        zip_close(zip);
                }
                if (zipParam.data != NULL) {
                        free(zipParam.data);
                }
                if (xmlParser != NULL) {
                        XML_ParserFree(xmlParser);
                }
                if (xmlParam.data != NULL) {
                        free(xmlParam.data);
                }
                if (xmlParam.path != NULL) {
                        delete xmlParam.path;
                }
                return NULL;
        } else if (stringEndsWith(file.c_str(), ".kml") == 1) {
                FILE *fp;
                int done;

                XML_Parser xmlParser;
                xmlParserKmlParam xmlParam;

                fp = NULL;
                done = 0;

                xmlParser = NULL;
                memset(&xmlParam, 0, sizeof(xmlParam));
                xmlParam.path = new DikePath();
                if (xmlParam.path == NULL) {
                        dikeErrorf("can not create path for file: %s", file.c_str());
                        goto kml_bail;
                }

                fp = fopen(file.c_str(), "rb");
                if (fp == NULL) {
                        dikeErrorf("can not open file: %s", file.c_str());
                }

                xmlParser = XML_ParserCreate(NULL);
                if (xmlParser == NULL) {
                        dikeErrorf("can not create parser");
                        goto kml_bail;
                }
                XML_SetUserData(xmlParser, &xmlParam);
                XML_SetElementHandler(xmlParser, xmlParserKmlStartElement, xmlParserKmlEndElement);
                XML_SetCharacterDataHandler(xmlParser, xmlParserKmlCharacterData);

                do {
                        void *const buf = XML_GetBuffer(xmlParser, 8192);
                        if (!buf) {
                                dikeErrorf("can not allocate memory");
                                goto kml_bail;
                        }

                        const size_t len = fread(buf, 1, 8192, fp);
                        if (ferror(fp)) {
                                dikeErrorf("can not allocate memory");
                                goto kml_bail;
                        }

                        done = feof(fp);

                        if (XML_ParseBuffer(xmlParser, (int) len, done) == XML_STATUS_ERROR) {
                                dikeErrorf("can not parse file: %s, line: %ld, error: %s", file.c_str(), XML_GetCurrentLineNumber(xmlParser), XML_ErrorString(XML_GetErrorCode(xmlParser)));
                                goto kml_bail;
                        }
                } while (!done);

                fclose(fp);
                free(xmlParam.data);
                XML_ParserFree(xmlParser);
                return xmlParam.path;
kml_bail:
                if (fp != NULL) {
                        fclose(fp);
                }
                if (xmlParser != NULL) {
                        XML_ParserFree(xmlParser);
                }
                if (xmlParam.data != NULL) {
                        free(xmlParam.data);
                }
                if (xmlParam.path != NULL) {
                        delete xmlParam.path;
                }
                return NULL;
        } else if (stringEndsWith(file.c_str(), ".gpx") == 1) {
                FILE *fp;
                int done;

                XML_Parser xmlParser;
                xmlParserGpxParam xmlParam;

                fp = NULL;
                done = 0;

                xmlParser = NULL;
                memset(&xmlParam, 0, sizeof(xmlParam));
                xmlParam.path = new DikePath();
                if (xmlParam.path == NULL) {
                        dikeErrorf("can not create path for file: %s", file.c_str());
                        goto gpx_bail;
                }

                fp = fopen(file.c_str(), "rb");
                if (fp == NULL) {
                        dikeErrorf("can not open file: %s", file.c_str());
                }

                xmlParser = XML_ParserCreate(NULL);
                if (xmlParser == NULL) {
                        dikeErrorf("can not create parser");
                        goto gpx_bail;
                }
                XML_SetUserData(xmlParser, &xmlParam);
                XML_SetElementHandler(xmlParser, xmlParserGpxStartElement, xmlParserGpxEndElement);
                XML_SetCharacterDataHandler(xmlParser, xmlParserGpxCharacterData);

                do {
                        void *const buf = XML_GetBuffer(xmlParser, 8192);
                        if (!buf) {
                                dikeErrorf("can not allocate memory");
                                goto gpx_bail;
                        }

                        const size_t len = fread(buf, 1, 8192, fp);
                        if (ferror(fp)) {
                                dikeErrorf("can not allocate memory");
                                goto gpx_bail;
                        }

                        done = feof(fp);

                        if (XML_ParseBuffer(xmlParser, (int) len, done) == XML_STATUS_ERROR) {
                                dikeErrorf("can not parse file: %s, line: %ld, error: %s", file.c_str(), XML_GetCurrentLineNumber(xmlParser), XML_ErrorString(XML_GetErrorCode(xmlParser)));
                                goto gpx_bail;
                        }
                } while (!done);

                fclose(fp);
                free(xmlParam.data);
                XML_ParserFree(xmlParser);
                return xmlParam.path;
gpx_bail:
                if (fp != NULL) {
                        fclose(fp);
                }
                if (xmlParser != NULL) {
                        XML_ParserFree(xmlParser);
                }
                if (xmlParam.data != NULL) {
                        free(xmlParam.data);
                }
                if (xmlParam.path != NULL) {
                        delete xmlParam.path;
                }
                return NULL;
        }
        dikeErrorf("file: %s is invalid", file.c_str());
        return NULL;
}

DikePath * DikePath::DikePathCreateFromBuffer (const char *buffer, int length)
{
        dikeDebugf("parsing buffer");
        if ((length >= 4) && (buffer[0] == 'P') && (buffer[1] == 'K') && (buffer[2] == 0x03) && (buffer[3] == 0x04)) {
                int rc;

                struct zip_t *zip;
                struct zipOnExtractParam zipParam;

                XML_Parser xmlParser;
                xmlParserKmlParam xmlParam;

                zip = NULL;
                memset(&zipParam, 0, sizeof(zipParam));

                xmlParser = NULL;
                memset(&xmlParam, 0, sizeof(xmlParam));

                zip = zip_stream_open(buffer, length, 0, 'r');
                if (zip == NULL) {
                        dikeErrorf("can not open zip");
                        goto kmz_bail;
                }
                rc = zip_entry_open(zip, "doc.kml");
                if (rc != 0) {
                        dikeErrorf("can not open zip entry: doc.kml");
                        goto kmz_bail;
                }
                zipParam.data = NULL;
                zipParam.size = 0;
                rc = zip_entry_extract(zip, zipOnExtract, &zipParam);
                if (rc != 0) {
                        dikeErrorf("can not extract zip entry: doc.kml");
                        goto kmz_bail;
                }

                xmlParam.path = new DikePath();
                if (xmlParam.path == NULL) {
                        dikeErrorf("can not create path for file");
                        goto kmz_bail;
                }

                xmlParser = XML_ParserCreate(NULL);
                if (xmlParser == NULL) {
                        dikeErrorf("can not create parser");
                        goto kmz_bail;
                }
                XML_SetUserData(xmlParser, &xmlParam);
                XML_SetElementHandler(xmlParser, xmlParserKmlStartElement, xmlParserKmlEndElement);
                XML_SetCharacterDataHandler(xmlParser, xmlParserKmlCharacterData);

                rc = XML_Parse(xmlParser, zipParam.data, zipParam.size, 1);
                if (rc != XML_STATUS_OK) {
                        dikeErrorf("can not parse line: %ld, error: %s", XML_GetCurrentLineNumber(xmlParser), XML_ErrorString(XML_GetErrorCode(xmlParser)));
                        goto kmz_bail;
                }

                free(xmlParam.data);
                XML_ParserFree(xmlParser);
                free(zipParam.data);
                zip_entry_close(zip);
                zip_stream_close(zip);
                return xmlParam.path;
kmz_bail:
                if (zip != NULL) {
                        zip_close(zip);
                }
                if (zipParam.data != NULL) {
                        free(zipParam.data);
                }
                if (xmlParser != NULL) {
                        XML_ParserFree(xmlParser);
                }
                if (xmlParam.data != NULL) {
                        free(xmlParam.data);
                }
                if (xmlParam.path != NULL) {
                        delete xmlParam.path;
                }
                return NULL;
        } else if (memmem(buffer, length, "<kml ", strlen("<kml ")) != NULL) {
                int rc;

                XML_Parser xmlParser;
                xmlParserKmlParam xmlParam;

                xmlParser = NULL;
                memset(&xmlParam, 0, sizeof(xmlParam));
                xmlParam.path = new DikePath();
                if (xmlParam.path == NULL) {
                        dikeErrorf("can not create path");
                        goto kml_bail;
                }

                xmlParser = XML_ParserCreate(NULL);
                if (xmlParser == NULL) {
                        dikeErrorf("can not create parser");
                        goto kml_bail;
                }
                XML_SetUserData(xmlParser, &xmlParam);
                XML_SetElementHandler(xmlParser, xmlParserKmlStartElement, xmlParserKmlEndElement);
                XML_SetCharacterDataHandler(xmlParser, xmlParserKmlCharacterData);

                rc = XML_Parse(xmlParser, buffer, length, 1);
                if (rc != XML_STATUS_OK) {
                        dikeErrorf("can not parse line: %ld, error: %s", XML_GetCurrentLineNumber(xmlParser), XML_ErrorString(XML_GetErrorCode(xmlParser)));
                        goto kml_bail;
                }

                free(xmlParam.data);
                XML_ParserFree(xmlParser);
                return xmlParam.path;
kml_bail:
                if (xmlParser != NULL) {
                        XML_ParserFree(xmlParser);
                }
                if (xmlParam.data != NULL) {
                        free(xmlParam.data);
                }
                if (xmlParam.path != NULL) {
                        delete xmlParam.path;
                }
                return NULL;
        } else if (memmem(buffer, length, "<gpx ", strlen("<gpx ")) != NULL) {
                int rc;

                XML_Parser xmlParser;
                xmlParserGpxParam xmlParam;

                xmlParser = NULL;
                memset(&xmlParam, 0, sizeof(xmlParam));
                xmlParam.path = new DikePath();
                if (xmlParam.path == NULL) {
                        dikeErrorf("can not create path");
                        goto gpx_bail;
                }

                xmlParser = XML_ParserCreate(NULL);
                if (xmlParser == NULL) {
                        dikeErrorf("can not create parser");
                        goto gpx_bail;
                }
                XML_SetUserData(xmlParser, &xmlParam);
                XML_SetElementHandler(xmlParser, xmlParserGpxStartElement, xmlParserGpxEndElement);
                XML_SetCharacterDataHandler(xmlParser, xmlParserGpxCharacterData);

                rc = XML_Parse(xmlParser, buffer, length, 1);
                if (rc != XML_STATUS_OK) {
                        dikeErrorf("can not parse line: %ld, error: %s", XML_GetCurrentLineNumber(xmlParser), XML_ErrorString(XML_GetErrorCode(xmlParser)));
                        goto gpx_bail;
                }
                free(xmlParam.data);
                XML_ParserFree(xmlParser);
                return xmlParam.path;
gpx_bail:
                if (xmlParser != NULL) {
                        XML_ParserFree(xmlParser);
                }
                if (xmlParam.data != NULL) {
                        free(xmlParam.data);
                }
                if (xmlParam.path != NULL) {
                        delete xmlParam.path;
                }
                return NULL;
        }
        dikeErrorf("buffer is invalid");
        return NULL;
}
