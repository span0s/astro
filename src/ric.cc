#include <iostream>
using namespace std;

#include "cmdline.h"
#include "io_ephemeris.h"

int main(int argc, const char* argv[]) {
    try {
        cmdline args(argc, argv,
            "Computes the RIC difference between two ephemeris files\n"

            "_Parameters\n"
            "  <ephem0> - Ephemeris file 0\n"
            "  <ephem1> - Ephemeris file 0\n"
            "  <outfile> - Output file for ric text output\n"
        );

        std::string ephem0_file = argv[1];
        std::string ephem1_file = argv[2];
        std::string outfile     = argv[3];

        Ephemeris ephem0 = readEphemAGI(ephem0_file);
        Ephemeris ephem1 = readEphemAGI(ephem1_file);

        vector<StateVec> ric = ephem0.RIC(ephem1);

        FILE* of = fopen(outfile.c_str(), "w");
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
