#ifndef ASTRO_EPHEMERIS_H
#define ASTRO_EPHEMERIS_H

#include "timecode.h"
#include "vecmat3.h"

class StateVec {
    public:
        StateVec(){}

        StateVec(Timecode tc, Vec3 pos) {
            tc_ = tc;
            pos_ = pos;
        }

        StateVec(Timecode tc, Vec3 pos, Vec3 vel) {
            tc_ = tc;
            pos_ = pos;
            vel_ = vel;
        }

        StateVec(Timecode tc, Vec3 pos, Vec3 vel, Vec3 acc) {
            tc_ = tc;
            pos_ = pos;
            vel_ = vel;
            acc_ = acc;
        }

        std::string getStr() {
            std::ostringstream strs;
            strs << "TC:  " << tc_.getStr() << "\nPos: " << pos_.getStr()
                 << "\nVel: " << vel_.getStr() << "\nAcc: " << acc_.getStr();
            return strs.str();
        }

        Timecode tc_;
        Vec3 pos_, vel_, acc_;
};

class Ephemeris {
    public:
        Ephemeris(){}

    public:
        std::vector<StateVec> states_;
};

#endif
