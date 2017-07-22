#ifndef ASTRO_EPHEMERIS_IO_H
#define ASTRO_EPHEMERIS_IO_H

#include <stdio.h>

bool writeEphemToAGI(std::string outfile, Ephemeris& ephem) {
    if (ephem.states_.size() == 0) return false;

    FILE* fp = fopen(outfile.c_str(), "w");

    fprintf(fp, "stk.v.4.3\n\n");
    fprintf(fp, "BEGIN Ephemeris\n\n");

    fprintf(fp, "ScenarioEpoch %s\n", ephem.states_[0].tc_.getStrAGI().c_str());

    std::string csystem = "UNKNOWN FRAME";
    switch(ephem.csystem_) {
        case FIXED    : csystem = "FIXED"; break;
        case INERTIAL : csystem = "ICRF";  break;
        case TEME     : csystem = "TEMEOfEpoch";  break;
    }
    fprintf(fp, "CoordinateSystem %s\n", csystem.c_str());
    if (ephem.csystem_ == TEME) {
        fprintf(fp, "CoordinateSystemEpoch %s\n", ephem.csystemEpoch_.getStrAGI().c_str());
    }

    fprintf(fp, "NumberOfEphemerisPoints %ld\n", ephem.states_.size());

    fprintf(fp, "\nEphemerisTimePosVel\n");
    for (int ii = 0; ii < (int)ephem.states_.size(); ii++) {
        fprintf(
            fp, "%.6lf %.6lf %.6lf %.6lf %.12lf %.12lf %.12lf\n",
            ephem.states_[ii].tc_ - ephem.states_[0].tc_,
            ephem.states_[ii].pos_.x_, ephem.states_[ii].pos_.y_, ephem.states_[ii].pos_.z_, 
            ephem.states_[ii].vel_.x_, ephem.states_[ii].vel_.y_, ephem.states_[ii].vel_.z_
        );
    }

    fprintf(fp, "\nEND Ephemeris\n");

    fclose(fp);

    return true;
}

#endif
