#ifndef ASTRO_VECMAT3_H
#define ASTRO_VECMAT3_H

#include <sstream>
#include <string>
#include <vector>
#include <math.h>

class Vec3 {
    public:
        Vec3() {
            x_ = 0;
            y_ = 0;
            z_ = 0;
        }

        Vec3(double xx, double yy, double zz) {
            x_ = xx;
            y_ = yy;
            z_ = zz;
        }

        double dot(const Vec3 aa) {
            return x_*aa.x_ + y_*aa.y_ + z_*aa.z_;
        }

        Vec3 cross(const Vec3 aa) {
            return Vec3(
                y_*aa.z_ - z_*aa.y_,
                z_*aa.x_ - x_*aa.z_,
                x_*aa.y_ - y_*aa.x_
            );
        }

        double mag() {
            return sqrt(x_*x_ + y_*y_ + z_*z_);
        }

        std::string getStr() {
            std::ostringstream strs;
            strs << "[" << x_ << ", " << y_ << ", " << z_ << "]";
            return strs.str();
        }

    public:
        Vec3 operator+(const Vec3 aa) {
            return Vec3(x_ + aa.x_, y_ + aa.y_, z_ + aa.z_);
        }
        Vec3 operator-(const Vec3 aa) {
            return Vec3(x_ - aa.x_, y_ - aa.y_, z_ - aa.z_);
        }
        Vec3 operator/(double aa) {
            return Vec3(x_/aa, y_/aa, z_/aa);
        }

    public:
        double x_, y_, z_;
};

const Vec3 operator*(const Vec3& vec, const double aa) {
    return Vec3(aa*vec.x_, aa*vec.y_, aa*vec.z_);
}
const Vec3 operator*(const double aa, const Vec3& vec) {
    return Vec3(aa*vec.x_, aa*vec.y_, aa*vec.z_);
}

class Mat3 {
    public:
        Mat3() {
            init();
        }

        Mat3(
            double a00, double a01, double a02,
            double a10, double a11, double a12,
            double a20, double a21, double a22
        ) {
            init();
            data_[0][0] = a00; data_[0][1] = a01; data_[0][2] = a02;
            data_[1][0] = a10; data_[1][1] = a11; data_[1][2] = a12;
            data_[2][0] = a20; data_[2][1] = a21; data_[2][2] = a22;
        }

        std::string getStr() {
            std::ostringstream strs;
            strs << data_[0][0] << ", " <<  data_[0][1] << ", " <<  data_[0][2] << std::endl;
            strs << data_[1][0] << ", " <<  data_[1][1] << ", " <<  data_[1][2] << std::endl;
            strs << data_[2][0] << ", " <<  data_[2][1] << ", " <<  data_[2][2] << std::endl;
            return strs.str();
        }

        Mat3 trans() {
            Mat3 ans(
                data_[0][0], data_[1][0], data_[2][0],
                data_[0][1], data_[1][1], data_[2][1],
                data_[0][2], data_[1][2], data_[2][2]
            );
            return ans;
        }

    public:
        std::vector<double>& operator[] (int index) {
            return data_[index];
        }

        Mat3 operator+(Mat3 aa) {
            Mat3 ans(
                data_[0][0] + aa[0][0], data_[0][1] + aa[0][1], data_[0][2] + aa[0][2],
                data_[1][0] + aa[1][0], data_[1][1] + aa[1][1], data_[1][2] + aa[1][2],
                data_[2][0] + aa[2][0], data_[2][1] + aa[2][1], data_[2][2] + aa[2][2]
            );
            return ans;
        }

        Mat3 operator-(Mat3 aa) {
            Mat3 ans(
                data_[0][0] - aa[0][0], data_[0][1] - aa[0][1], data_[0][2] - aa[0][2],
                data_[1][0] - aa[1][0], data_[1][1] - aa[1][1], data_[1][2] - aa[1][2],
                data_[2][0] - aa[2][0], data_[2][1] - aa[2][1], data_[2][2] - aa[2][2]
            );
            return ans;
        }

        Vec3 operator*(Vec3 aa) {
            Vec3 ans(
                data_[0][0]*aa.x_ + data_[0][1]*aa.y_ + data_[0][2]*aa.z_,
                data_[1][0]*aa.x_ + data_[1][1]*aa.y_ + data_[1][2]*aa.z_,
                data_[2][0]*aa.x_ + data_[2][1]*aa.y_ + data_[2][2]*aa.z_
            );
            return ans;
        }

        Mat3 operator*(Mat3 aa) {
            return Mat3(
                data_[0][0]*aa[0][0] + data_[0][1]*aa[1][0] + data_[0][2]*aa[2][0],
                data_[0][0]*aa[0][1] + data_[0][1]*aa[1][1] + data_[0][2]*aa[2][1],
                data_[0][0]*aa[0][2] + data_[0][1]*aa[1][2] + data_[0][2]*aa[2][2],
                data_[1][0]*aa[0][0] + data_[1][1]*aa[1][0] + data_[1][2]*aa[2][0],
                data_[1][0]*aa[0][1] + data_[1][1]*aa[1][1] + data_[1][2]*aa[2][1],
                data_[1][0]*aa[0][2] + data_[1][1]*aa[1][2] + data_[1][2]*aa[2][2],
                data_[2][0]*aa[0][0] + data_[2][1]*aa[1][0] + data_[2][2]*aa[2][0],
                data_[2][0]*aa[0][1] + data_[2][1]*aa[1][1] + data_[2][2]*aa[2][1],
                data_[2][0]*aa[0][2] + data_[2][1]*aa[1][2] + data_[2][2]*aa[2][2]
            );
        }

    private:
        void init() {
            for (int ii = 0; ii < 3; ii++) {
                std::vector<double> tmp;
                for (int jj = 0; jj < 3; jj++) {
                    tmp.push_back(0);
                }
                data_.push_back(tmp);
            } 
        }
    
    public:
        std::vector< std::vector<double> > data_;
};

const Mat3 operator*(Mat3& mat, const double aa) {
    return Mat3(
        aa*mat[0][0], aa*mat[0][1], aa*mat[0][2],
        aa*mat[1][0], aa*mat[1][1], aa*mat[1][2],
        aa*mat[2][0], aa*mat[2][1], aa*mat[2][2]
    );
}
const Mat3 operator*(const double aa, Mat3& mat) {
    return Mat3(
        aa*mat[0][0], aa*mat[0][1], aa*mat[0][2],
        aa*mat[1][0], aa*mat[1][1], aa*mat[1][2],
        aa*mat[2][0], aa*mat[2][1], aa*mat[2][2]
    );
}

#endif
