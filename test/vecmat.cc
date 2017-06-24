#include <iostream>
#include "vecmat3.h"

using namespace std;

int main(int argc, char *argv[]) {
    Vector3 tmp;
    Vector3 tmp2(1,2,3);
    Vector3 tmp3(4,5,6);

    cout << tmp.getStr() << endl;
    cout << tmp2.getStr() << endl;
    cout << tmp3.getStr() << endl;

    cout << (tmp2 + tmp3).getStr() << endl;
    cout << (tmp3 - tmp2*2).getStr() << endl;
    cout << (tmp3 - 2*tmp2).getStr() << endl;
    cout << (tmp2 / 2).getStr() << endl;


    Mat3 mat;
    cout << mat.getStr() << endl;
    mat[1][1] = 4;
    cout << mat.getStr() << endl;
    cout << mat[0][1] << " " << mat[1][1] << endl;

    return 0;
}
