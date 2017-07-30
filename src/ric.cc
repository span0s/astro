#include <iostream>
using namespace std;

#include "io_ephemeris.h"

int main(int argc, char* argv[]) {
    Ephemeris ephem0 = readEphemAGI(argv[1]);
    Ephemeris ephem1 = readEphemAGI(argv[2]);

    vector<StateVec> ric = ephem0.RIC(ephem1);
    
    return 0;
}
