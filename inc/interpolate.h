#ifndef ASTRO_INTERPOLATE_H
#define ASTRO_INTERPOLATE_H

#include <vector>

std::vector<double> divDiff(std::vector<double> fx, std::vector<double> xx) {
    
    for (int ii = 0; ii < (int)xx.size(); ii++) {
        for (int jj = xx.size()-1; jj >= ii; jj--) {
            fx[jj] = (fx[jj] - fx[jj-1]) / (xx[jj] - xx[jj-1]);
        }
    }

    return fx;
}

double evalInterp(std::vector<double> ddiff, std::vector<double> xx, double tt) {
    int nn = xx.size();
    double ans = ddiff[xx[nn-1]];
    for (int ii = nn-1; ii >= 0; ii--) {
        ans = ddiff[ii] + (tt - xx[ii])*ans;
    }

    return ans;
}

#endif
