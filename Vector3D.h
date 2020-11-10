#pragma once
#ifndef VECTOR3D_H
#define VECTOR3D_H
#include "common.h"
#include "cmath"
#include "string"

class Vector3{
    public:
        double i, j, k;
        double norm;
        Vector3(double i, double j, double k);
        double dot(Vector3 &vec);
        Vector3 cross(Vector3 &vec);
        vector<double> getAngles();
    private:
};
#endif