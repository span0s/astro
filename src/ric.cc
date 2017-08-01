#include <iostream>
using namespace std;

#include "io_ephemeris.h"

int main(int argc, char* argv[]) {
    try {

    Ephemeris ephem0 = readEphemAGI(argv[1]);
    Ephemeris ephem1 = readEphemAGI(argv[2]);

    vector<StateVec> ric = ephem0.RIC(ephem1);

    FILE* of = fopen(argv[3], "w");
    for (int ii = 0; ii < (int)ric.size(); ii++) {
        fprintf(
            of, "%s %lf %lf %lf %lf %lf %lf\n", ric[ii].tc_.getStr().c_str(),
            ric[ii][0], ric[ii][1], ric[ii][2], ric[ii][3], ric[ii][4], ric[ii][5]
        );
    }
    fclose(of);

    } catch (const char* ee) {
        cout << ee << endl;
    }
    
    return 0;
}
