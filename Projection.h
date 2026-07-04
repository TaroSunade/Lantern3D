#pragma once
#include "Transform.h"

class CProjection {
public:
    CTransform mat;
    void SetPerspective(double fovDeg, double aspect, double nearZ, double farZ) {
        mat.Identity();
        double f = 1.0 / tan(fovDeg * 3.141592653589793 / 360.0);
        double dz = farZ - nearZ;
        mat.m[0][0] = f / aspect;
        mat.m[1][1] = f;
        mat.m[2][2] = -(farZ + nearZ) / dz;
        mat.m[2][3] = -2.0 * farZ * nearZ / dz;
        mat.m[3][2] = -1.0;
        mat.m[3][3] = 0;
    }
    void Project(double x, double y, double z, int& sx, int& sy, double& depth,
                 int scrW, int scrH) const {
        double w = mat.m[3][0]*x + mat.m[3][1]*y + mat.m[3][2]*z + mat.m[3][3];
        if (fabs(w) < 1e-10) w = 1e-10;
        double nx = (mat.m[0][0]*x + mat.m[0][1]*y + mat.m[0][2]*z + mat.m[0][3]) / w;
        double ny = (mat.m[1][0]*x + mat.m[1][1]*y + mat.m[1][2]*z + mat.m[1][3]) / w;
        double nz = (mat.m[2][0]*x + mat.m[2][1]*y + mat.m[2][2]*z + mat.m[2][3]) / w;
        sx = (int)((nx + 1.0) * 0.5 * scrW);
        sy = (int)((1.0 - ny) * 0.5 * scrH);
        depth = nz;
    }
};
