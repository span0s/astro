#include <iostream>
using namespace std;

#include "string_extra.h"

int main(int argc, char* argv[]) {
    string str = "HI         there    mom  ";
    vector<string> tmp = strSplit(str, ' ');
    for (int ii = 0; ii < (int)tmp.size(); ii++) {
        cout << tmp[ii] << endl;
    }

    str = "HI    ,    there   , mom  ";
    tmp = strSplit(str, ',');
    for (int ii = 0; ii < (int)tmp.size(); ii++) {
        cout << tmp[ii] << endl;
    }
    return 0;
}
