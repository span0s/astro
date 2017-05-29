#ifndef ASTRO_TIMECODE_H_
#define ASTRO_TIMECODE_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

struct DateTime{
    int32_t year, month, day;
    int32_t hour, min;
    double secs;
};

// ref: github/xscott/xmtools
class Timecode {
    public:
        Timecode(int whole, double fract) {
            this->whole = whole;
            this->fract = fract;
            normalize();
        }
        Timecode(int year, int month, int day) {
            init(year, month, day, 0, 0, 0);
        }
        Timecode(int year, int month, int day, int hour, int minutes, double secs) {
            init(year, month, day, hour, minutes, secs);
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

    private:
        void init(int year, int month, int day, int hour, int minutes, double secs) {
            // TODO: Check inputs

            static const int32_t moffset[] = {
                0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
            };

            whole = (year - 1950) * 365;  // add days in first
            whole += (year - 1949)/4;     // leap days since epoch
            whole += moffset[month - 1];  // days in current year

            if ((year % 4 == 0) && (month > 2)) {
                whole++;                  // possible current leap day
            }

            whole += day-1;   whole *= 24; // day of the month, now at curent day
            whole += hour;    whole *= 60; // hours
            whole += minutes; whole *= 60; // minutes
            fract = secs;
            normalize();
        }

        void normalize() {
            double lower = floor(fract);
            whole += (int64_t)lower;
            fract -= lower;
        }

        DateTime getDt(double places = -1) {
            normalize();
            int before_whole = whole;
            int before_fract = fract;

            if (places >= 0) {
            double scale = pow(10, places);
                fract = round(fract*scale)/scale;
                if (fract == 1.0) {
                    whole += 1;
                    fract = 0.0;
                }
            }

            static const int32_t mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

            int32_t sec  = whole % 60; whole /= 60;
            int32_t min  = whole % 60; whole /= 60;
            int32_t hour = whole % 24; whole /= 24;

            // whole is now days since 1950, switch to 1948 to make math easier
            whole += 365+366;
            int32_t year = 1948 + whole/(365*4+1)*4; whole %= (365*4+1);

            // handle remainder of years
            int32_t curleap=1;
            if (whole > 365) {
                year += 1;         whole -= 366;
                year += whole/365; whole %= 365;
                curleap=0;
            }

            size_t ii;
            for (ii = 0; ii < 12; ii++) {
                int32_t days = mdays[ii] + (((ii == 1) && curleap) ? 1 : 0);
                if (whole < days) {
                    break;
                }
                whole -= days;
            }
       
            int32_t day   = whole+1;
            int32_t month = ii + 1;

            whole = before_whole;
            fract = before_fract;
            return (DateTime){year, month, day, hour, min, sec + fract};
        }

    public:
        Timecode operator+(double aa) {
            return Timecode(this->whole, this->fract + aa);
        }
        Timecode operator-(double aa) {
            return Timecode(this->whole, this->fract - aa);
        }

    private:
        int whole;
        double fract;
};

#endif
