
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tuple>
#include <vector>

#define DIKE_DEBUG_NAME "brute"
#include "DikeDebug.hpp"
#include "DikePath.hpp"
#include "DikeMethod.hpp"
#include "DikeMethodBrute.hpp"

class DikeMethodBrutePrivate {
public:
        DikeMethodBrutePrivate (void);
        ~DikeMethodBrutePrivate (void);

        int addTrack (DikePath *path);
        int addRecord (DikePath *path);

        double calculate (void);

private:
        std::vector<DikePath *> _tracks;
        std::vector<DikePath *> _records;
};

DikeMethodBrutePrivate::DikeMethodBrutePrivate (void)
{
}

DikeMethodBrutePrivate::~DikeMethodBrutePrivate (void)
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

int DikeMethodBrutePrivate::addTrack (DikePath *path)
{
        _tracks.push_back(path->incref());
        return 0;
}

int DikeMethodBrutePrivate::addRecord (DikePath *path)
{
        _records.push_back(path->incref());
        return 0;
}

double DikeMethodBrutePrivate::calculate (void)
{
        int i, il;
        int j, jl;
        int k, kl;
        int l, ll;
        double dist;

        int pts;
        int mts;

        pts = 0;
        mts = 0;

        dikeDebugf("calculating tracks:");
        for (i = 0, il = _tracks.size(); i < il; i++) {
                dikeDebugf("  - %d points", _tracks[i]->getPointsCount());
                pts +=  _tracks[i]->getPointsCount();
                for (j = 0, jl = _tracks[i]->getPointsCount(); j < jl; j++) {
                        std::tuple<DikePath::Command, DikePoint> *tpoint;
                        tpoint = _tracks[i]->getPoint(j);
                        for (k = 0, kl = _records.size(); k < kl; k++) {
                                dikeDebugf("  - %d points", _records[k]->getPointsCount());
                                for (l = 0, ll = _records[k]->getPointsCount(); l < ll; l++) {
                                        std::tuple<DikePath::Command, DikePoint> *rpoint;
                                        rpoint = _records[k]->getPoint(l);
                                        dikeDebugf("  - j: %d, l: %d", j, l);
                                        dist = DikePoint::DikePointDistanceEuclidean(&std::get<1>(*tpoint), &std::get<1>(*rpoint));
                                        if (dist <= 200.00) {
                                                mts += 1;
                                                break;
                                        }
                                }
                                if (l < ll) {
                                        break;
                                }
                        }
                }
        }

        return (mts * 100.00) / pts;
}

DikeMethodBrute::DikeMethodBrute (void)
{
        _private = new DikeMethodBrutePrivate();
}

DikeMethodBrute::~DikeMethodBrute (void)
{
        if (_private != NULL) {
                delete _private;
        }
}

int DikeMethodBrute::addTrack (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addTrack(path);
bail:   return -1;
}

int DikeMethodBrute::addRecord (DikePath *path)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->addRecord(path);
bail:   return -1;
}

double DikeMethodBrute::calculate (void)
{
        if (_private == NULL) {
                dikeErrorf("private is invalid");
                goto bail;
        }
        return _private->calculate();
bail:   return -1;
}
