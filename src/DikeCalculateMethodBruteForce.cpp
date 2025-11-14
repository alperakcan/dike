
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tuple>
#include <vector>
#include <memory>

#define DIKE_DEBUG_NAME "bruteforce"
#include "DikeDebug.hpp"
#include "DikePath.hpp"
#include "DikeCalculateMethod.hpp"
#include "DikeCalculateMethodBruteForce.hpp"

class DikeCalculateMethodBruteForcePrivate {
public:
        DikeCalculateMethodBruteForcePrivate (void);
        ~DikeCalculateMethodBruteForcePrivate (void);

        int addTrack (DikePath *path);
        int addRecord (DikePath *path);
        int setCoverageRadius (int radius);

        std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>  calculate (void);

private:
        std::vector<DikePath *> _tracks;
        std::vector<DikePath *> _records;
        int _coverageRadius;
};

DikeCalculateMethodBruteForcePrivate::DikeCalculateMethodBruteForcePrivate (void)
{
}

DikeCalculateMethodBruteForcePrivate::~DikeCalculateMethodBruteForcePrivate (void)
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

int DikeCalculateMethodBruteForcePrivate::addTrack (DikePath *path)
{
        _tracks.push_back(path->incref());
        return 0;
}

int DikeCalculateMethodBruteForcePrivate::addRecord (DikePath *path)
{
        _records.push_back(path->incref());
        return 0;
}

int DikeCalculateMethodBruteForcePrivate::setCoverageRadius (int radius)
{
        _coverageRadius = radius;
        return 0;
}

std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>  DikeCalculateMethodBruteForcePrivate::calculate (void)
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

        DikePath *trackAccepted;
        DikePath *trackRejected;

        pts = 0;
        mpts = 0;
        pmpts = 0;

        dts = 0;
        mdts = 0;

        trackAccepted = NULL;
        trackRejected = NULL;

        trackAccepted = new DikePath();
        if (trackAccepted == NULL) {
                dikeErrorf("can not create trackAccepted");
                goto bail;
        }
        trackRejected = new DikePath();
        if (trackRejected == NULL) {
                dikeErrorf("can not create trackRejected");
                goto bail;
        }

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
                                        if (dist <= _coverageRadius) {
                                                break;
                                        }
                                }
                                if (l < ll) {
                                        break;
                                }
                        }

                        if (k < kl) {
                                if (pmpts) {
                                        mpts += 1;
                                        mdts += (ppoint == NULL) ? 0 : DikePoint::DikePointDistanceEuclidean(&std::get<1>(*ppoint), &std::get<1>(*tpoint));
                                } else if (j == 0) {
                                        mpts += 1;
                                }

                                if (ppoint != NULL) {
                                        trackAccepted->addPoint(DikePath::CommandMoveTo, std::get<1>(*ppoint));
                                        trackAccepted->addPoint(DikePath::CommandLineTo, std::get<1>(*tpoint));
                                }

                                pmpts = 1;
                        } else {
                                if (ppoint != NULL) {
                                        trackRejected->addPoint(DikePath::CommandMoveTo, std::get<1>(*ppoint));
                                        trackRejected->addPoint(DikePath::CommandLineTo, std::get<1>(*tpoint));
                                }

                                pmpts = 0;
                        }
                }
        }

        return std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>(0, mpts, pts, mdts, dts, trackAccepted, trackRejected);
bail:   if (trackAccepted != NULL) {
                trackAccepted->decref();
        }
        if (trackRejected != NULL) {
                trackRejected->decref();
        }
        return std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>(-1, 0, 0, 0, 0, nullptr, nullptr);
}

DikeCalculateMethodBruteForce::DikeCalculateMethodBruteForce (const DikeCalculateMethodOptions &options)
        : DikeCalculateMethod(options)
{
        _private = new DikeCalculateMethodBruteForcePrivate();
        _private->setCoverageRadius(getCoverageRadius());
}

DikeCalculateMethodBruteForce::~DikeCalculateMethodBruteForce (void)
{
        if (_private != NULL) {
                delete _private;
        }
}

int DikeCalculateMethodBruteForce::addTrack (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addTrack(path);
bail:   return -1;
}

int DikeCalculateMethodBruteForce::addRecord (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addRecord(path);
bail:   return -1;
}

std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>  DikeCalculateMethodBruteForce::calculate (void)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->calculate();
bail:   return std::tuple<int, int, int, double, double, std::unique_ptr<DikePath>, std::unique_ptr<DikePath>>(-1, 0, 0, 0, 0, nullptr, nullptr);
}
