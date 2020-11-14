#include "common.h"
#include "cmath"
#include "string"
#include "Vector3D.h"

Vector3 :: Vector3(double i, double j, double k, double x, double y, double z) : i(i), j(j), k(k), x(x), y(y), z(z), norm(sqrt(i*i + j*j + k*k)){

}

double Vector3 :: dot(Vector3 &vec){
    return i*vec.i + j*vec.j + k*vec.k;
}

Vector3 Vector3 :: cross(Vector3 &vec){
    return Vector3(j*vec.k-k*vec.j, k*vec.i-i*vec.k, i*vec.j-j*vec.i, x, y, z);
}

vector<double> Vector3 :: getAngles(){
    return {acos(i/norm), acos(j/norm), acos(k/norm)};
}

