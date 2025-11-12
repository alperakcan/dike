
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include <string>
#include <memory>
#include <vector>

#define DIKE_DEBUG_NAME "DikeInflate"
#include "DikeDebug.hpp"
#include "DikeMethod.hpp"
#include "DikePath.hpp"

#define OPTION_HELP                     'h'
#define OPTION_DEBUG                    'd'
#define OPTION_METHOD                   'm'
#define OPTION_TRACK                    't'
#define OPTION_OUTPUT                   'o'
#define OPTION_COVERAGE_RADIUS          'c'

#define OPTION_DEBUG_DEFAULT            "info"
#define OPTION_OUTPUT_DEFAULT           "stdout"
#define OPTION_COVERAGE_RADIUS_DEFAULT  250

static struct option long_options[] = {
        { "help",               no_argument,            NULL,   OPTION_HELP             },
        { "debug",              required_argument,      NULL,   OPTION_DEBUG            },
        { "method",             required_argument,      NULL,   OPTION_METHOD           },
        { "track",              required_argument,      NULL,   OPTION_TRACK            },
        { "output",             required_argument,      NULL,   OPTION_OUTPUT           },
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
        fprintf(stdout, "  -o, --output         : output file (default: %s)\n", OPTION_OUTPUT_DEFAULT);
        fprintf(stdout, "  -c, --coverageRadius : coverage radius (default: %d meters)\n", OPTION_COVERAGE_RADIUS_DEFAULT);
        fprintf(stdout, "  -h, --help           : this text\n");
}

int main (int argc, char **argv)
{
        int rs;

        int o;
        int oi;

        int i;
        int j;
        int jl;
        std::string o_method;
        std::vector<std::string> o_tracks;
        int o_coverageRadius;
        std::string o_output;

        enum {
                OUTPUT_FORMAT_INVALID,
                OUTPUT_FORMAT_GPX,
                OUTPUT_FORMAT_KML
        };

        int ofmt;
        FILE *ofp;
        std::vector<std::unique_ptr<DikePath>> tpaths;
        std::vector<std::unique_ptr<DikePath>> ipaths;

        rs = 0;

        ofp = stdout;
        ofmt = OUTPUT_FORMAT_INVALID;
        o_coverageRadius = OPTION_COVERAGE_RADIUS_DEFAULT;

        DikeDebugInit();
        DikeDebugSetLevel(DikeDebugLevelFromString(OPTION_DEBUG_DEFAULT));

        while ((o = getopt_long(argc, argv, "hd:m:t:o:c:", long_options, &oi)) != -1) {
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
                        case OPTION_OUTPUT:
                                o_output = optarg;
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

        dikeInfof("loading:");
        for (i = 0; i < (int) o_tracks.size(); i++) {
                DikePath *path;
                dikeInfof("  track: %s", o_tracks[i].c_str());
                path = DikePath::DikePathCreateFromFile(o_tracks[i]);
                if (path == NULL) {
                        dikeErrorf("can not create path: %s", o_tracks[i].c_str());
                        goto bail;
                }
                dikeInfof("    pointsCount: %d", path->getPointsCount());
                tpaths.push_back(std::unique_ptr<DikePath>(path));
        }

        dikeInfof("inflating:");
        for (i = 0; i < (int) tpaths.size(); i++) {
                DikePath *path;
                DikePath *ipath;
                dikeInfof("  track: %s", o_tracks[i].c_str());
                path = tpaths[i].get();
                ipath = DikePath::DikePathCreateInflatedFromPath(path, o_coverageRadius);
                if (ipath == NULL) {
                        dikeErrorf("can not inflate path: %s", o_tracks[i].c_str());
                        goto bail;
                }
                dikeInfof("    pointsCount: %d", ipath->getPointsCount());
                ipaths.push_back(std::unique_ptr<DikePath>(ipath));
        }

        if (o_output == "" || o_output == "-" || o_output == "stdout") {
                o_output = "stdout";
        }
        if (o_output == "stdout") {
                ofp = stdout;
        } else {
                ofp = fopen(o_output.c_str(), "wb");
                if (ofp == NULL) {
                        dikeErrorf("can not open output file: %s", o_output.c_str());
                        goto bail;
                }
        }

        if (o_output.size() >= 4 && o_output.compare(o_output.size() - 4, 4, ".gpx") == 0) {
                ofmt = OUTPUT_FORMAT_GPX;
        } else if (o_output.size() >= 4 && o_output.compare(o_output.size() - 4, 4, ".kml") == 0) {
                ofmt = OUTPUT_FORMAT_KML;
        }

        if (ofmt == OUTPUT_FORMAT_GPX) {
                fprintf(ofp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
                fprintf(ofp, "<gpx version=\"1.1\" creator=\"DikeInflate\">\n");
        } else if (ofmt == OUTPUT_FORMAT_KML) {
                fprintf(ofp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
                fprintf(ofp, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");

                fprintf(ofp, "<Document>\n");
                fprintf(ofp, "  <name>%s</name>\n", o_output.c_str());
        }

        dikeInfof("writing: %s", o_output.c_str());
        for (i = 0; i < (int) ipaths.size(); i++) {
                int m;
                DikePath *ipath;

                dikeInfof("  track: %s", o_tracks[i].c_str());

                ipath = ipaths[i].get();

                if (ofmt == OUTPUT_FORMAT_KML) {
                        fprintf(ofp, "  <Placemark>\n");
                        fprintf(ofp, "    <name>%s (inflated %d meters)</name>\n", o_tracks[i].c_str(), o_coverageRadius);
                        fprintf(ofp, "    <MultiGeometry>\n");
                }

                for (m = 0, j = 0, jl = ipath->getPointsCount(); j < jl; j++) {
                        std::tuple<DikePath::Command, DikePoint> *point;
                        point = ipath->getPoint(j);
                        if (std::get<0>(*point) == DikePath::CommandMoveTo) {
                                if (ofmt == OUTPUT_FORMAT_GPX) {
                                        if (m) {
                                                fprintf(ofp, "    </trkseg>\n");
                                                fprintf(ofp, "  </trk>\n");
                                        }
                                        fprintf(ofp, "  <trk>\n");
                                        fprintf(ofp, "    <name>%s (inflated %d meters)</name>\n", o_tracks[i].c_str(), o_coverageRadius);
                                        fprintf(ofp, "    <trkseg>\n");
                                } else if (ofmt == OUTPUT_FORMAT_KML) {
                                        if (m) {
                                                fprintf(ofp, "        </coordinates>\n");
                                                fprintf(ofp, "      </LineString>\n");

                                        }
                                        fprintf(ofp, "      <LineString>\n");
                                        fprintf(ofp, "        <tessellate>1</tessellate>\n");
                                        fprintf(ofp, "        <coordinates>\n");
                                }
                                m = 1;
                        }
                        if (ofmt == OUTPUT_FORMAT_GPX) {
                                fprintf(ofp, "      <trkpt lat=\"%.8f\" lon=\"%.8f\"></trkpt>\n", std::get<1>(*point).lat(), std::get<1>(*point).lon());
                        } else if (ofmt == OUTPUT_FORMAT_KML) {
                                fprintf(ofp, "        %.8f,%.8f,0\n", std::get<1>(*point).lon(), std::get<1>(*point).lat());
                        }
                }
                if (ofmt == OUTPUT_FORMAT_GPX) {
                        if (m) {
                                fprintf(ofp, "    </trkseg>\n");
                                fprintf(ofp, "  </trk>\n");
                        }
                } else if (ofmt == OUTPUT_FORMAT_KML) {
                        if (m) {
                                fprintf(ofp, "        </coordinates>\n");
                                fprintf(ofp, "      </LineString>\n");
                                fprintf(ofp, "    </MultiGeometry>\n");
                                fprintf(ofp, "  </Placemark>\n");
                        }
                }
        }

        if (ofmt == OUTPUT_FORMAT_GPX) {
                fprintf(ofp, "</gpx>\n");
        } else if (ofmt == OUTPUT_FORMAT_KML) {
                fprintf(ofp, "</Document>\n");
                fprintf(ofp, "</kml>\n");
        }
        if (ofp != stdout) {
                fclose(ofp);
        }
out:
        DikeDebugFini();
        return rs;

bail:
        if (ofp != stdout) {
                unlink(o_output.c_str());
        }
        rs = -1;
        goto out;
}
