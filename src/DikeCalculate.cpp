
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <string>
#include <vector>

#define DIKE_DEBUG_NAME "DikeCalculate"
#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikePath.hpp"

#define OPTION_HELP                     'h'
#define OPTION_DEBUG                    'd'
#define OPTION_METHOD                   'm'
#define OPTION_TRACK                    't'
#define OPTION_RECORD                   'r'
#define OPTION_COVERAGE_RADIUS          'c'

#define OPTION_DEBUG_DEFAULT            "info"
#define OPTION_METHOD_DEFAULT           "quadtree"
#define OPTION_COVERAGE_RADIUS_DEFAULT  250

static struct option long_options[] = {
        { "help",               no_argument,            NULL,   OPTION_HELP             },
        { "debug",              required_argument,      NULL,   OPTION_DEBUG            },
        { "method",             required_argument,      NULL,   OPTION_METHOD           },
        { "track",              required_argument,      NULL,   OPTION_TRACK            },
        { "record",             required_argument,      NULL,   OPTION_RECORD           },
        { "coverageRadius",     required_argument,      NULL,   OPTION_COVERAGE_RADIUS  }
};

static void print_usage (const char *pname)
{
        fprintf(stdout, "Usage: %s [OPTION]...\n", pname);
        fprintf(stdout, "  -d, --debug          : debug level (default: %s)\n", OPTION_DEBUG_DEFAULT);
        fprintf(stdout, "                         silent\n");
        fprintf(stdout, "                         error\n");
        fprintf(stdout, "                         warning\n");
        fprintf(stdout, "                         notice\n");
        fprintf(stdout, "                         info\n");
        fprintf(stdout, "                         debug\n");
        fprintf(stdout, "                         trace\n");
        fprintf(stdout, "  -m, --method         : method to calculate truth (default: %s)\n", OPTION_METHOD_DEFAULT);
        fprintf(stdout, "                         bruteforce\n");
        fprintf(stdout, "                         quadtree\n");
        fprintf(stdout, "  -t, --track          : base route to compare with\n");
        fprintf(stdout, "  -r, --record         : recorded route to calculate truth\n");
        fprintf(stdout, "  -c, --coverageRadius : coverage radius (default: %d meters)\n", OPTION_COVERAGE_RADIUS_DEFAULT);
        fprintf(stdout, "  -h, --help           : this text\n");
}

int main (int argc, char **argv)
{
        int rc;
        int rs;

        int o;
        int oi;

        int i;
        std::string o_method;
        std::vector<std::string> o_tracks;
        std::vector<std::string> o_records;
        int o_coverageRadius;

        DikeMethod *method;
        DikeMethodOptions options;
        std::tuple<int, int, int, double, double> calc;

        rs = 0;
        method = NULL;

        o_method        = OPTION_METHOD_DEFAULT;
        o_coverageRadius= OPTION_COVERAGE_RADIUS_DEFAULT;

        DikeDebugInit();
        DikeDebugSetLevel(DikeDebugLevelFromString(OPTION_DEBUG_DEFAULT));

        while ((o = getopt_long(argc, argv, "hd:m:t:r:c:", long_options, &oi)) != -1) {
                switch (o) {
                        case 'h':
                                print_usage(argv[0]);
                                goto out;
                        case OPTION_DEBUG:
                                DikeDebugSetLevel(optarg);
                                break;
                        case OPTION_METHOD:
                                o_method = optarg;
                                break;
                        case OPTION_TRACK:
                                o_tracks.push_back(optarg);
                                break;
                        case OPTION_RECORD:
                                o_records.push_back(optarg);
                                break;
                        case OPTION_COVERAGE_RADIUS:
                                o_coverageRadius = atoi(optarg);
                                break;
                }
        }

        if (o_tracks.size() <= 0) {
                dikeErrorf("tracks is invalid, see help");
                goto bail;
        }
        if (o_records.size() <= 0) {
                dikeErrorf("records is invalid, see help");
                goto bail;
        }

        dikeInfof("options:");
        dikeInfof("  method : %s", o_method.c_str());
        dikeInfof("  tracks :");
        for (i = 0; i < (int) o_tracks.size(); i++) {
                dikeInfof("    - %s", o_tracks[i].c_str());
        }
        dikeInfof("  records:");
        for (i = 0; i < (int) o_records.size(); i++) {
                dikeInfof("    - %s", o_records[i].c_str());
        }

        options = DikeMethodOptions();
        options.coverageRadius = o_coverageRadius;

        method = DikeMethod::DikeMethodCreateWithType(o_method, options);
        if (method == NULL) {
                dikeErrorf("can not create method: %s", o_method.c_str());
                goto bail;
        }

        for (i = 0; i < (int) o_tracks.size(); i++) {
                DikePath *path;
                dikeInfof("loading track: %s", o_tracks[i].c_str());
                path = DikePath::DikePathCreateFromFile(o_tracks[i]);
                if (path == NULL) {
                        dikeErrorf("can not create path: %s", o_tracks[i].c_str());
                        goto bail;
                }
                dikeInfof("  pointsCount: %d", path->getPointsCount());
                rc = method->addTrack(path);
                if (rc < 0) {
                        dikeErrorf("can not add track: %s", o_tracks[i].c_str());
                        path->decref();
                        goto bail;
                }
                path->decref();
        }

        for (i = 0; i < (int) o_records.size(); i++) {
                DikePath *path;
                dikeInfof("loading record: %s", o_records[i].c_str());
                path = DikePath::DikePathCreateFromFile(o_records[i]);
                if (path == NULL) {
                        dikeErrorf("can not create path: %s", o_records[i].c_str());
                        goto bail;
                }
                dikeInfof("  pointsCount: %d", path->getPointsCount());
                rc = method->addRecord(path);
                if (rc < 0) {
                        dikeErrorf("can not add track: %s", o_records[i].c_str());
                        path->decref();
                        goto bail;
                }
                path->decref();
        }

        dikeInfof("calculating:");
        calc = method->calculate();
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

out:
        if (method != NULL) {
                delete method;
        }
        DikeDebugFini();
        return rs;

bail:
        rs = -1;
        goto out;
}
