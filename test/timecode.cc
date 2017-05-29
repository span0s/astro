#include <iostream>
using namespace std;

#include "timecode.h"

int main(int argc, char *argv[]) {
    Timecode tc = Timecode(2000, 1, 2, 3, 4, 5.67890123456);
    cout << tc.getStr() << endl;

    Timecode tc1 = tc + 10;
    cout << tc1.getStr() << endl;
    Timecode tc2 = tc - 10;
    cout << tc2.getStr() << endl;

    return 0;
}
