#pragma once
#include "Vec3.h"

class CTransform {
public:
    double m[4][4];

    CTransform() { Identity(); }

    void Identity() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0 : 0.0;
    }

    static CTransform Multiply(const CTransform& a, const CTransform& b) {
        CTransform r;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                r.m[i][j] = 0;
                for (int k = 0; k < 4; k++)
                    r.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        return r;
    }

    Vec3 TransformPoint(const Vec3& v) const {
        double x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3];
        double y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3];
        double z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3];
        double w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3];
        if (fabs(w) > 1e-10) { x /= w; y /= w; z /= w; }
        return Vec3(x, y, z);
    }

    Vec3 TransformNormal(const Vec3& n) const {
        double x = m[0][0]*n.x + m[0][1]*n.y + m[0][2]*n.z;
        double y = m[1][0]*n.x + m[1][1]*n.y + m[1][2]*n.z;
        double z = m[2][0]*n.x + m[2][1]*n.y + m[2][2]*n.z;
        return Vec3(x, y, z).Normalize();
    }

    static CTransform Translate(double tx, double ty, double tz) {
        CTransform r; r.m[0][3] = tx; r.m[1][3] = ty; r.m[2][3] = tz; return r;
    }
    static CTransform Scale(double sx, double sy, double sz) {
        CTransform r; r.m[0][0] = sx; r.m[1][1] = sy; r.m[2][2] = sz; return r;
    }
    static CTransform RotateX(double a) {
        CTransform r; double c = cos(a), s = sin(a);
        r.m[1][1] = c; r.m[1][2] = -s; r.m[2][1] = s; r.m[2][2] = c; return r;
    }
    static CTransform RotateY(double a) {
        CTransform r; double c = cos(a), s = sin(a);
        r.m[0][0] = c; r.m[0][2] = s; r.m[2][0] = -s; r.m[2][2] = c; return r;
    }
    static CTransform RotateZ(double a) {
        CTransform r; double c = cos(a), s = sin(a);
        r.m[0][0] = c; r.m[0][1] = -s; r.m[1][0] = s; r.m[1][1] = c; return r;
    }
};
