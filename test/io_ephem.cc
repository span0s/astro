#include <iostream>
#include <vector>
using namespace std;

#include "sgp4.h"
#include "io_ephemeris.h"

int main(int argc, char *argv[]) {

    string str1 = "1 28868U 05036A   17189.60254437 -.00000076 +00000-0 +00000-0 0  9997";
    string str2 = "2 28868 000.0215 332.1778 0003279 099.8260 324.3314 01.00271962013763";
    TLE tle = TLE(str1, str2);

    Ephemeris ephem = ephemFromTLE(tle, tle.epoch_, tle.epoch_ + 86400, 60);

    writeEphemToAGI("tmp.e", ephem);

    return 0;
}
