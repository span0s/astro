#ifndef ASTRO_EPHEM_GEN_H
#define ASTRO_EPHEM_GEN_H

#include "sgp4.h"
#include "ephemeris.h"

Ephemeris ephemFromTLE(TLE tle, Timecode tc0, Timecode tc1, double dt) {
    Ephemeris ephem;
    ephem.csystem_ = TEME;
    ephem.csystemEpoch_ = tle.epoch_;

    int count = 0;
    Timecode tc = tc0;
    while (tc <= tc1) {
        ephem.states_.push_back(tle.getState(tc));
        count++;

        // Ensure last point is created
        Timecode next = tc0 + count*dt;
        if (tc < tc1 && next > tc1) {
            tc = tc1;
        } else {
            tc = next;
        }
    }

    return ephem;
}

#endif
