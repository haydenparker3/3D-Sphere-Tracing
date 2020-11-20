#include "common.h"
#include "cmath"
#include "string"
#include "Vector3D.h"

Vector3 :: Vector3(double i, double j, double k, double x, double y, double z) : i(i), j(j), k(k), x(x), y(y), z(z), norm(sqrt(i*i + j*j + k*k)){

}

void Vector3 :: normalize(){
    double denom = sqrt(i*i + j*j + k*k);
    i = i/denom;
    j = j/denom;
    k = k/denom;
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

Vector3 Vector3 :: operator *(double mult){
    return Vector3(i*mult, j*mult, k*mult, x, y, z);
}

Vector3 Vector3 :: operator +(Vector3 &obj){
    return Vector3(i+obj.i, j+obj.j, k+obj.k, x, y, z);
}

