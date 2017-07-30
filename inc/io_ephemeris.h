#ifndef ASTRO_EPHEMERIS_IO_H
#define ASTRO_EPHEMERIS_IO_H

#include <stdio.h>
#include <fstream>
#include <sstream>
#include "string_extra.h"

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

Ephemeris readEphemAGI(std::string filename) {
    Ephemeris ephem;

    std::ifstream infile(filename.c_str());

    bool epochFound = false;
    bool csystemFound = false;
    bool csystemEpochFound = false;
    bool atEphemLines = false;
    bool foundEnd = false;

    Timecode epoch;

    // Parse line by line
    std::string line;
    while (std::getline(infile, line)) {
        if (!epochFound && line.find("ScenarioEpoch") != std::string::npos) {
            epochFound = true;
            std::vector<std::string> tmp = strSplit(line, ' ');
            if (tmp.size() != 2) {
                throw "Invalid \"ScenarioEpoch\" line in AGI ephem file";
            }
            epoch = Timecode::parseAGI(tmp[1]);
        }
        if (!csystemFound && line.find("CoordinateSystem") != std::string::npos) {
            csystemFound = true;
            std::vector<std::string> tmp = strSplit(line, ' ' );
        }
        if (!csystemEpochFound && line.find("CoordinateSystemEpoch") != std::string::npos) {
            csystemEpochFound = true;
        }

        if (line.find("EphemerisTimePosVel") != std::string::npos) {
            atEphemLines = true;
        }

        if (line.find("END Ephemeris") != std::string::npos) {
            foundEnd = true;
            break;
        }

        if (atEphemLines) {

        }
    }

    infile.close();

    // Invalid inputs
    if (!epochFound)
        throw "Failed to find \"ScenarioEpoch\" when reading AGI ephemeris file";
    if (!csystemFound)
        throw "Failed to find \"CoordinateSystem\" when reading AGI ephemeris file";
    if (ephem.csystem_ == TEME && !csystemEpochFound)
        throw "Failed to find \"CoordinateSystemEpoch\" for frame that needs it";
    if (!foundEnd)
        throw "Failed to find \"END Ephemeris\"";

    return ephem;
}

#endif
