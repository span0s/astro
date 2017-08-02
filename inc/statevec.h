#ifndef ASTRO_STATEVEC_H
#define ASTRO_STATEVEC_H

#include "timecode.h"
#include "vecmat3.h"

class StateVec {
    public:
        StateVec(){}

        StateVec(const StateVec& sv) {
            tc_ = sv.tc_;
            pos_ = sv.pos_;
            vel_ = sv.vel_;
            acc_ = sv.acc_;
        }

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

        Mat3 ricMat() {
            Vec3 rHat = pos_.norm();
            Vec3 hHat = (pos_.cross(vel_)).norm();
            Vec3 aHat = (hHat.cross(rHat)).norm();

            return Mat3(
                rHat.x_, rHat.y_, rHat.z_,
                aHat.x_, aHat.y_, aHat.z_,
                hHat.x_, hHat.y_, hHat.z_
            );
        }

        StateVec ricDelta(StateVec other) {
            if (tc_ != other.tc_) throw "StateVec times must match for RIC";

            Mat3 mat = ricMat();
            Vec3 r_ric = mat*(other.pos_ - pos_);
            Vec3 v_ric = mat*(other.vel_ - vel_);

            return StateVec(tc_, r_ric, v_ric);
        }

        double operator [](int idx) const {
            if (idx >=0 && idx <= 2) {
                return pos_[idx];
            } else if (idx >= 3 && idx <= 5) {
                return vel_[idx-3];
            } else if (idx >= 6 && idx <= 8) {
                return acc_[idx-6];
            }
            throw "StateVec index outside of bounds";
        }
        double& operator [](int idx) {
            if (idx >=0 && idx <= 2) {
                return pos_[idx];
            } else if (idx >= 3 && idx <= 5) {
                return vel_[idx-3];
            } else if (idx >= 6 && idx <= 8) {
                return acc_[idx-6];
            }
            throw "StateVec index outside of bounds";
        }

        friend bool operator <(const StateVec& aa, const StateVec& bb) {
            return aa.tc_ < bb.tc_;
        }

    public:
        Timecode tc_;
        Vec3 pos_, vel_, acc_;
};

#endif
