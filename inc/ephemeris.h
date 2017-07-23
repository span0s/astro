#ifndef ASTRO_EPHEMERIS_H
#define ASTRO_EPHEMERIS_H

#include "timecode.h"
#include "vecmat3.h"
#include "interpolate.h"

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

        double operator [](int idx) const {
            if (idx >=0 && idx <= 2) {
                return pos_[idx];
            } else if (idx >= 3 && idx <= 5) {
                return vel_[idx-3];
            } else if (idx >= 6 && idx <= 8) {
                return acc_[idx-6];
            }
            throw std::string("StateVec index outside of bounds");
        }
        double& operator [](int idx) {
            if (idx >=0 && idx <= 2) {
                return pos_[idx];
            } else if (idx >= 3 && idx <= 5) {
                return vel_[idx-3];
            } else if (idx >= 6 && idx <= 8) {
                return acc_[idx-6];
            }
            throw std::string("StateVec index outside of bounds");
        }

    public:
        Timecode tc_;
        Vec3 pos_, vel_, acc_;
};

enum CoordSystem {
    FIXED = 0,
    INERTIAL,
    TEME
};

class Ephemeris {
    public:
        Ephemeris() {
            accValid = false;    
        }

        int stateVecSize() {
            if (accValid) return 9;
            return 6;
        }

        /**
         * Interpolates the ephemeris to the given time using lagrange interpolation
         *
         * @param Time to interpolate to
         * @param Number of points to use in interpolate
         *
         * @return The interpolated state at the given time
         */
        StateVec getSV(Timecode tc, int numpts = 4) {
            if (states_.size() == 0) throw std::string("No ephemeris points to interpolate");

            // Find bounding indices
            int idx = 0;
            bool found = false;
            while (idx < (int)states_.size()-1) {
                if (states_[idx].tc_ < tc && states_[idx].tc_ > tc) {
                    found = true;
                    break;
                }
                idx++;
            }
            if (!found) throw std::string("Requested time outside ephemeris time span");

            // XXX Handle odd number of points better
            int idx_lo = idx - floor(numpts/2.0);
            int idx_hi = idx + floor(numpts/2.0);
            if (idx_lo < 0) idx_lo = 0;
            if (idx_hi > (int)states_.size()-1) idx_hi = states_.size()-1;

            // XXX Handle edge cases
            
            StateVec ans;
            ans.tc_ = tc;
            // Interpolate all elements individually
            for (int ii = 0; ii < stateVecSize(); ii++) {
                std::vector<double> fx;
                std::vector<double> xx;
                for (int jj = idx_lo; jj <= idx_hi; jj++) {
                    xx.push_back(states_[jj].tc_ - states_[0].tc_);
                    fx.push_back(states_[jj][ii]);
                }

                ans[ii] = evalInterp(divDiff(fx, xx), xx, tc - states_[0].tc_);
            }

            return ans;
        }

        /**
         * Interpolates the current ephemeris to the given timestep using
         * lagrange interpolation
         *  
         * @param time step to interpolate to
         * @param number of points to use in interpolation
         *
         * @return Ephemeris interpolated to given time step
         */
        Ephemeris interpToStep(double step, int numpts = 4) {
            Ephemeris ephem;

            int count = 0;
            Timecode tc0 = states_.front().tc_;
            Timecode tc1 = states_.back().tc_;
            Timecode tc = tc0;
            while (tc <= tc1) {
                ephem.states_.push_back(getSV(tc, numpts));

                count++;
                Timecode next = tc0 + count*step;
                if (tc < tc1 && next > tc1) {
                    tc = tc1;
                } else {
                    tc = next;
                }
            }
            return ephem;
        }

    public:
        std::vector<StateVec> states_;
        bool accValid;

        CoordSystem csystem_;
        Timecode csystemEpoch_;
};

#endif
