#ifndef ASTRO_TIMECODE_H
#define ASTRO_TIMECODE_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string>
#include <vector>

struct DateTime{
    int32_t year, month, day;
    int32_t hour, min;
    double secs;
};

// ref: github/xscott/xmtools
class Timecode {
    public:
        Timecode() {
            whole_ = 0;
            fract_ = 0;
        }
        Timecode(int whole, double fract) {
            whole_ = whole;
            fract_ = fract;
            normalize();
        }
        Timecode(int year, int month, int day) {
            init(year, month, day, 0, 0, 0);
        }
        Timecode(int year, int month, int day, int hour, int minutes, double secs) {
            init(year, month, day, hour, minutes, secs);
        }   
        Timecode(DateTime dt) {
            init(dt.year, dt.month, dt.day, dt.hour, dt.min, dt.secs);
        }

        std::string getStr() {
            DateTime dt = getDt();
            char buffer[32];
            sprintf(
                buffer, "%04d-%02d-%02d::%02d:%02d:%09.6f",
                dt.year, dt.month, dt.day, dt.hour, dt.min, dt.secs
            );

            std::string str = buffer;
                
            return str;
        }

        std::string getStrAGI() {
            DateTime dt = getDt();
            char buffer[32];
            sprintf(
                buffer, "%02d %s %04d %02d:%02d:%09.6f",
                dt.day, monthStrs_[dt.month-1], dt.year,
                dt.hour, dt.min, dt.secs
            );

            std::string str = buffer;
                
            return str;
        }

    private:
        void init(int year, int month, int day, int hour, int minutes, double secs) {
            // TODO: Check inputs

            static const int32_t moffset[] = {
                0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
            };

            whole_ = (year - 1950) * 365;  // add days in first
            whole_ += (year - 1949)/4;     // leap days since epoch
            whole_ += moffset[month - 1];  // days in current year

            if ((year % 4 == 0) && (month > 2)) {
                whole_++;                  // possible current leap day
            }

            whole_ += day-1;   whole_ *= 24; // day of the month, now at curent day
            whole_ += hour;    whole_ *= 60; // hours
            whole_ += minutes; whole_ *= 60; // minutes
            fract_ = secs;
            normalize();
        }

        void normalize() {
            double lower = floor(fract_);
            whole_ += (int64_t)lower;
            fract_ -= (int64_t)lower;
        }

        DateTime getDt(double places = -1) {
            normalize();
            int before_whole = whole_;
            double before_fract = fract_;

            if (places >= 0) {
            double scale = pow(10, places);
                fract_ = round(fract_*scale)/scale;
                if (fract_ == 1.0) {
                    whole_ += 1;
                    fract_ = 0.0;
                }
            }

            static const int32_t mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

            int32_t sec  = whole_ % 60; whole_ /= 60;
            int32_t min  = whole_ % 60; whole_ /= 60;
            int32_t hour = whole_ % 24; whole_ /= 24;

            // whole is now days since 1950, switch to 1948 to make math easier
            whole_ += 365+366;
            int32_t year = 1948 + whole_/(365*4+1)*4; whole_ %= (365*4+1);

            // handle remainder of years
            int32_t curleap=1;
            if (whole_ > 365) {
                year += 1;         whole_ -= 366;
                year += whole_/365; whole_ %= 365;
                curleap=0;
            }

            size_t ii;
            for (ii = 0; ii < 12; ii++) {
                int32_t days = mdays[ii] + (((ii == 1) && curleap) ? 1 : 0);
                if (whole_ < days) {
                    break;
                }
                whole_ -= days;
            }
       
            int32_t day   = whole_+1;
            int32_t month = ii + 1;

            whole_ = before_whole;
            fract_ = before_fract;
            return (DateTime){year, month, day, hour, min, sec + fract_};
        }

    public:
        Timecode operator +(double aa) {
            return Timecode(whole_, fract_ + aa);
        }

        Timecode operator -(double aa) {
            return Timecode(whole_, fract_ - aa);
        }

        double operator -(const Timecode& tc) {
            return (double)(whole_ - tc.whole_) + (fract_ - tc.fract_);
        }

        void operator +=(double aa) {
            this->fract_ += aa;
            normalize();
        }

        void operator -=(double aa) {
            this->fract_ -= aa;
            normalize();
        }

        friend bool operator <(Timecode& aa, Timecode& bb) {
            return (aa - bb) < 0;
        }
        friend bool operator <=(Timecode& aa, Timecode& bb) {
            return (aa - bb) <= 0;
        }

        friend bool operator >(Timecode& aa, Timecode& bb) {
            return (aa - bb) > 0;
        }
        friend bool operator >=(Timecode& aa, Timecode& bb) {
            return (aa - bb) >= 0;
        }

        friend bool operator ==(const Timecode& aa, Timecode& bb) {
            return (aa.whole_ == bb.whole_) && (aa.fract_ == bb.fract_);
        }

        static const char* const monthStrs_[];

    private:
        int whole_;
        double fract_;
};

const char* const Timecode::monthStrs_[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "June",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

#endif
