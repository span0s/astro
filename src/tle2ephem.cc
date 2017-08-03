#include <iostream>

using namespace std;

#include "cmdline.h"
#include "ephem_gen.h"
#include "io_ephemeris.h"

int main(int argc, const char* argv[]) {
    try {
        cmdline args(argc, argv,
            "Generates an ephemeris file from a TLE\n"

            "_Parameters\n"
            "  <tlefile> - File with TLE(s) in it\n"
            "  <outfile> - Output ephemeris file\n"
            "  <starttime> - Time to start ephemeris at \"YYYY:MM:DD::HH:MM::SS.SSS\"\n"
            "  <stoptime>  - Time to stop ephemeris at \"YYYY:MM:DD::HH:MM::SS.SSS\"\n"
            "_Options\n"
            "  --satid - Satellite ID to get matching TLE for (Defaults to first TLE in file)\n"
        );
                    
    } catch(const char* ee) {
        cout << ee << endl;
    }

    return 0;
}
