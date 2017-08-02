#include <iostream>
#include <vector>
using namespace std;

#include "ephem_gen.h"

int main(int argc, char *argv[]) {

    string str1 = "1 28868U 05036A   17189.60254437 -.00000076 +00000-0 +00000-0 0  9997";
    string str2 = "2 28868 000.0215 332.1778 0003279 099.8260 324.3314 01.00271962013763";

    vector<char> cstr1(str1.c_str(), str1.c_str() + str1.size() + 1);
    vector<char> cstr2(str2.c_str(), str2.c_str() + str2.size() + 1);

    Vallado::elsetrec satrec;
    double startmfe, stopmfe, deltamin;
    Vallado::gravconsttype whichconst = Vallado::wgs72;

    char typerun = 'c';
    char typeinput = 'm';
    Vallado::twoline2rv(
        cstr1.data(), cstr2.data(), typerun, typeinput, 'a', whichconst,
        startmfe, stopmfe, deltamin, satrec
    );

    cout << satrec.satnum
         << " " << satrec.classification
         << " " << satrec.intldesg
         << " " << satrec.epochyr
         << " " << satrec.epochdays
         << " " << satrec.ndot
         << " " << satrec.nddot
         << " " << satrec.bstar
         << " " << satrec.ephtype
         << " " << satrec.elnum
         << endl;

    cout << satrec.satnum
         << " " << satrec.inclo
         << " " << satrec.nodeo
         << " " << satrec.ecco
         << " " << satrec.argpo
         << " " << satrec.mo
         << " " << satrec.no_kozai
         << " " << satrec.revnum
         << endl;

    double pos[3];
    double vel[3];
    Vallado::sgp4(satrec, 1.0, pos, vel);
    cout << pos[0] << ", " << pos[1] << ", " << pos[2] << endl;
    cout << vel[0] << ", " << vel[1] << ", " << vel[2] << endl;

    TLE tle = TLE(str1, str2);
    StateVec sv = tle.getState(tle.epoch_ + 1);
    cout << sv.getStr() << endl;

    Ephemeris ephem = ephemFromTLE(tle, tle.epoch_, tle.epoch_ + 86400, 60);
    for (int ii = 0; ii < (int)ephem.states_.size(); ii++) {
        cout << ephem.states_[ii].getStr() << endl;
    }

    return 0;
}
