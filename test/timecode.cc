#include <iostream>
using namespace std;

#include "timecode.h"

int main(int argc, char *argv[]) {
    Timecode tc = Timecode(2000, 1, 2, 3, 4, 5.67890123456);
    cout << "tc = " << tc.getStr() << " = 2000-01-02::03:04:05.678901" << endl;

    Timecode tc1 = tc + 10;
    cout << "tc1 = tc + 10 sec = " << tc1.getStr() << endl;
    Timecode tc2 = tc - 10;
    cout << "tc2 = tc - 10 sec = " << tc2.getStr() << endl;

    cout << "tc1 - tc = " << tc1 - tc << endl;
    cout << "tc - tc1 = " << tc - tc1 << endl;

    tc += 10;
    cout << "tc = " << tc.getStr() << endl;
    tc -= 10;
    cout << "tc = " << tc.getStr() << endl;

    bool ans0 = tc1 > tc;
    bool ans1 = tc2 < tc;
    bool ans2 = tc == tc;
    bool ans3 = tc1 == tc2;
    cout << "1 1 1 0 == " << ans0 << " " << ans1 << " " << ans2 << " " << ans3 << endl;

    return 0;
}
