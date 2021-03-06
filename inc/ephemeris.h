#ifndef ASTRO_EPHEMERIS_H
#define ASTRO_EPHEMERIS_H

#include <set>
#include <algorithm>

#include "statevec.h"
#include "interpolate.h"

enum CoordSystem {
    FIXED = 0,
    INERTIAL,
    TEME,
    J2000
};

class Ephemeris {
    public:
        Ephemeris() {
            csystem_ = FIXED;
            accValid_ = false;    
        }

        /**
         * Gets the size of each state vec in the ephemeris
         *
         * @return The number of elements in each state vec
         */
        int stateVecSize() {
            if (accValid_) return 9;
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
            if (states_.size() == 0) throw "No ephemeris points to interpolate";

            // Find bounding indices
            int idx = 0;
            bool found = false;
            while (idx < (int)states_.size()-1) {
                if (states_[idx].tc_ == tc) return states_[idx];
                if (states_[idx+1].tc_ == tc) return states_[idx+1];

                if (states_[idx].tc_ <= tc && states_[idx+1].tc_ >= tc) {
                    found = true;
                    break;
                }
                idx++;
            }
            if (!found) throw "Requested time outside ephemeris time span";

            // XXX Handle odd number of points better
            int idx_lo = idx - floor(numpts/2.0) + 1;
            int idx_hi = idx + floor(numpts/2.0);
            if (idx_lo < 0) idx_lo = 0;
            if (idx_hi > (int)states_.size()-1) idx_hi = states_.size()-1;

            // XXX Handle edge cases
            int tmp_numpts = idx_hi - idx_lo + 1;
            if (tmp_numpts != numpts) {
                if (idx_lo == 0) idx_hi += numpts - tmp_numpts;
                if (idx_hi == (int)states_.size()-1) idx_lo -= numpts - tmp_numpts;
            }
            
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
            ephem.accValid_ = accValid_;
            ephem.csystem_ = csystem_;
            ephem.csystemEpoch_ = csystemEpoch_;

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

        std::vector<StateVec> RIC(Ephemeris& ephem) {
            Timecode tc0 = states_.front().tc_;
            Timecode tc1 = states_.back().tc_;
            if (ephem.states_.front().tc_ > tc0) tc0 = ephem.states_.front().tc_;
            if (ephem.states_.back().tc_ < tc1) tc1 = ephem.states_.back().tc_;

            std::vector<StateVec> ans;
            std::set<Timecode> times;
            for (int ii = 0; ii < (int)states_.size(); ii++) {
                times.insert(states_[ii].tc_);
                StateVec other = ephem.getSV(states_[ii].tc_);
                ans.push_back(states_[ii].ricDelta(other));
            }

            for (int ii = 0; ii < (int)ephem.states_.size(); ii++) {
                if (times.count(ephem.states_[ii].tc_) == 1) continue;
                
                times.insert(ephem.states_[ii].tc_);
                StateVec ref = getSV(ephem.states_[ii].tc_);
                ans.push_back(ref.ricDelta(ephem.states_[ii]));
            }

            std::sort(ans.begin(), ans.end());

            return ans;
        }

    public:
        std::vector<StateVec> states_;
        bool accValid_;

        CoordSystem csystem_;
        Timecode csystemEpoch_;
};

#endif
