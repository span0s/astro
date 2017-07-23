#include <iostream>
#include "vecmat3.h"

using namespace std;

int main(int argc, char *argv[]) {
    Vec3 tmp;
    Vec3 tmp2(1,2,3);
    Vec3 tmp3(4,5,6);

    cout << tmp.getStr() << endl;
    cout << tmp2.getStr() << endl;
    tmp2[0] = 10;
    cout << tmp2[0] << " " << tmp2[1] << " " << tmp2[2] << endl;
    cout << tmp3.getStr() << endl;

    cout << (tmp2 + tmp3).getStr() << endl;
    cout << (tmp3 - tmp2*2).getStr() << endl;
    cout << (tmp3 - 2*tmp2).getStr() << endl;
    cout << (tmp2 / 2).getStr() << endl;


    Mat3 mat(1,2,3,4,5,6,7,8,9);
    Mat3 mat1(11,22,33,44,55,66,77,88,99);
    cout << endl;
    cout << (mat*mat1).getStr() << endl;
    cout << endl;
    cout << mat.getStr() << endl;
    cout << (mat*tmp2).getStr() << endl;
    mat[1][1] = 1234;
    cout << mat.getStr() << endl;
    cout << mat.trans().getStr() << endl;
    cout << mat[0][1] << " " << mat[1][1] << endl;

    Mat3 asdf = mat*3;
    Mat3 qwer = 3*mat;
    cout << mat.getStr() << endl;
    cout << asdf.getStr() << endl;

    return 0;
}
