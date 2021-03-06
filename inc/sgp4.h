#ifndef ASTRO_SGP4_H
#define ASTRO_SGP4_H

#include "vallado_sgp4.h"
#include "statevec.h"

class TLE{
    public:
        TLE(std::string line0, std::string line1) {
            // Store/convert line data
            line0_ = line0;
            line1_ = line1;
            vector<char> cstr1(line0.c_str(), line0.c_str() + line0.size() + 1);
            vector<char> cstr2(line1.c_str(), line1.c_str() + line1.size() + 1);

            // Vallado inputs
            char typerun = 'c';
            char typeinput = 'm';
            Vallado::gravconsttype whichconst = Vallado::wgs72;

            // Initialize element set
            double startmfe, stopmfe, deltamin;
            Vallado::twoline2rv(
                cstr1.data(), cstr2.data(), typerun, typeinput, 'a', whichconst,
                startmfe, stopmfe, deltamin, satrec_
            );

            // Set epoch
            DateTime dt;
            Vallado::invjday(
                satrec_.jdsatepoch, satrec_.jdsatepochF,
                dt.year, dt.month, dt.day, dt.hour, dt.min, dt.secs
            );
            epoch_ = Timecode(dt);
        }

        StateVec getState(Timecode tc) {
            double pos[3], vel[3];
            Vallado::sgp4(satrec_, (tc - epoch_)/60.0, pos, vel);
            return StateVec(
                tc, Vec3(pos[0], pos[1], pos[2])*1000.0,
                Vec3(vel[0], vel[1], vel[2])*1000.0
            );
        }

    public:
        string line0_, line1_;
        Timecode epoch_;
        Vallado::elsetrec satrec_;
};

// Load TLE's from files
// Find TLE's for satid

#endif
