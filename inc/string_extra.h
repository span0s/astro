#ifndef ASTRO_STRING_EXTRA_H
#define ASTRO_STRING_EXTRA_H

#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> strSplit(const std::string& str, char delim) {
    std::vector<std::string> vals;
    std::size_t start = 0, end = 0;
    while ((end = str.find(delim, start)) != std::string::npos) {
        if (end != start) {
            vals.push_back(str.substr(start, end - start));
        }
        start = end + 1;
    }
    std::string tmp = str.substr(start, end - start);
    if (tmp.find_first_not_of(' ') != std::string::npos) {
        vals.push_back(str.substr(start));
    }
    return vals;
}

#endif
