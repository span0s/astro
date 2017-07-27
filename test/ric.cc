#include <iostream>
using namespace std;

#include "sgp4.h"

int main(int argc, char* argv[]) {
    string str1 = "1 28868U 05036A   17189.60254437 -.00000076 +00000-0 +00000-0 0  9997";
    string str2 = "2 28868 000.0215 332.1778 0003279 099.8260 324.3314 01.00271962013763";
    TLE tle = TLE(str1, str2);
    Ephemeris ephem0 = ephemFromTLE(tle, tle.epoch_, tle.epoch_ + 86400, 60);
    Ephemeris ephem1 = ephem0.interpToStep(10);

    std::vector<StateVec> RIC = ephem0.RIC(ephem1);

    for (int ii = 0; ii < (int)RIC.size(); ii++) {
        cout << RIC[ii].getStr() << endl;
    }

    return 0;
}
