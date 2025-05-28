
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tuple>
#include <vector>

#define DIKE_DEBUG_NAME "bruteforce"
#include "DikeDebug.hpp"
#include "DikePath.hpp"
#include "DikeMethod.hpp"
#include "DikeMethodBruteForce.hpp"

class DikeMethodBruteForcePrivate {
public:
        DikeMethodBruteForcePrivate (void);
        ~DikeMethodBruteForcePrivate (void);

        int addTrack (DikePath *path);
        int addRecord (DikePath *path);

        std::tuple<int, int, int, double, double>  calculate (void);

private:
        std::vector<DikePath *> _tracks;
        std::vector<DikePath *> _records;
};

DikeMethodBruteForcePrivate::DikeMethodBruteForcePrivate (void)
{
}

DikeMethodBruteForcePrivate::~DikeMethodBruteForcePrivate (void)
{
        int i;
        int il;
        for (i = 0, il = _tracks.size(); i < il; i++) {
                _tracks[i]->decref();
        }
        for (i = 0, il = _records.size(); i < il; i++) {
                _records[i]->decref();
        }
}

int DikeMethodBruteForcePrivate::addTrack (DikePath *path)
{
        _tracks.push_back(path->incref());
        return 0;
}

int DikeMethodBruteForcePrivate::addRecord (DikePath *path)
{
        _records.push_back(path->incref());
        return 0;
}

std::tuple<int, int, int, double, double>  DikeMethodBruteForcePrivate::calculate (void)
{
        int i, il;
        int j, jl;
        int k, kl;
        int l, ll;
        double dist;

        int pts;
        int mpts;
        int pmpts;

        double dts;
        double mdts;

        pts = 0;
        mpts = 0;
        pmpts = 0;

        dts = 0;
        mdts = 0;

        dikeDebugf("calculating tracks:");
        for (i = 0, il = _tracks.size(); i < il; i++) {
                dikeDebugf("  - %d points", _tracks[i]->getPointsCount());
                for (j = 0, jl = _tracks[i]->getPointsCount(); j < jl; j++) {
                        std::tuple<DikePath::Command, DikePoint> *tpoint;
                        std::tuple<DikePath::Command, DikePoint> *ppoint;

                        ppoint = (j == 0) ? NULL : _tracks[i]->getPoint(j - 1);
                        tpoint = _tracks[i]->getPoint(j);

                        pts += 1;
                        dts += (ppoint == NULL) ? 0 : DikePoint::DikePointDistanceEuclidean(&std::get<1>(*ppoint), &std::get<1>(*tpoint));

                        for (k = 0, kl = _records.size(); k < kl; k++) {
                                dikeDebugf("    - %d points", _records[k]->getPointsCount());
                                for (l = 0, ll = _records[k]->getPointsCount(); l < ll; l++) {
                                        std::tuple<DikePath::Command, DikePoint> *rpoint;
                                        rpoint = _records[k]->getPoint(l);
                                        dikeDebugf("      - j: %d, l: %d", j, l);
                                        dist = DikePoint::DikePointDistanceEuclidean(&std::get<1>(*tpoint), &std::get<1>(*rpoint));
                                        if (dist <= 250.00) {
                                                if (pmpts) {
                                                        mpts += 1;
                                                        mdts += (ppoint == NULL) ? 0 : DikePoint::DikePointDistanceEuclidean(&std::get<1>(*ppoint), &std::get<1>(*tpoint));
                                                }
                                                break;
                                        }
                                }
                                if (l < ll) {
                                        break;
                                }
                        }

                        if (k < kl) {
                                pmpts = 1;
                        } else {
                                pmpts = 0;
                        }
                }
        }

        return std::tuple<int, int, int, double, double>(0, mpts, pts, mdts, dts);
}

DikeMethodBruteForce::DikeMethodBruteForce (const DikeMethodOptions &options)
        : DikeMethod(options)
{
        _private = new DikeMethodBruteForcePrivate();
}

DikeMethodBruteForce::~DikeMethodBruteForce (void)
{
        if (_private != NULL) {
                delete _private;
        }
}

int DikeMethodBruteForce::addTrack (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addTrack(path);
bail:   return -1;
}

int DikeMethodBruteForce::addRecord (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addRecord(path);
bail:   return -1;
}

std::tuple<int, int, int, double, double>  DikeMethodBruteForce::calculate (void)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->calculate();
bail:   return std::tuple<int, int, int, double, double>(-1, 0, 0, 0, 0);
}
