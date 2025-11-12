
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <string>
#include <vector>

#define DIKE_DEBUG_NAME "DikeInflate"
#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikePath.hpp"

#define OPTION_HELP                     'h'
#define OPTION_DEBUG                    'd'
#define OPTION_METHOD                   'm'
#define OPTION_TRACK                    't'
#define OPTION_COVERAGE_RADIUS          'c'

#define OPTION_DEBUG_DEFAULT            "info"
#define OPTION_COVERAGE_RADIUS_DEFAULT  250

static struct option long_options[] = {
        { "help",               no_argument,            NULL,   OPTION_HELP             },
        { "debug",              required_argument,      NULL,   OPTION_DEBUG            },
        { "method",             required_argument,      NULL,   OPTION_METHOD           },
        { "track",              required_argument,      NULL,   OPTION_TRACK            },
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
        fprintf(stdout, "  -t, --track          : base route to compare with\n");
        fprintf(stdout, "  -c, --coverageRadius : coverage radius (default: %d meters)\n", OPTION_COVERAGE_RADIUS_DEFAULT);
        fprintf(stdout, "  -h, --help           : this text\n");
}

int main (int argc, char **argv)
{
        int rs;

        int o;
        int oi;

        int i;
        std::string o_method;
        std::vector<std::string> o_tracks;
        int o_coverageRadius;

        rs = 0;

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
                        case OPTION_COVERAGE_RADIUS:
                                o_coverageRadius = atoi(optarg);
                                break;
                }
        }

        if (o_tracks.size() <= 0) {
                dikeErrorf("tracks is invalid, see help");
                goto bail;
        }

        dikeInfof("options:");
        dikeInfof("  method : %s", o_method.c_str());
        dikeInfof("  tracks :");
        for (i = 0; i < (int) o_tracks.size(); i++) {
                dikeInfof("    - %s", o_tracks[i].c_str());
        }
        dikeInfof("  coverageRadius: %d",  o_coverageRadius);

        for (i = 0; i < (int) o_tracks.size(); i++) {
                DikePath *path;
                dikeInfof("loading track: %s", o_tracks[i].c_str());
                path = DikePath::DikePathCreateFromFile(o_tracks[i]);
                if (path == NULL) {
                        dikeErrorf("can not create path: %s", o_tracks[i].c_str());
                        goto bail;
                }
                dikeInfof("  pointsCount: %d", path->getPointsCount());
                path->decref();
        }

        dikeInfof("inflating:");

out:
        DikeDebugFini();
        return rs;

bail:
        rs = -1;
        goto out;
}
