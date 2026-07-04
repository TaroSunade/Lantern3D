#pragma once
#include "Vec3.h"
#include <algorithm>
#include <cmath>

class CMaterial {
public:
    Vec3 ambient, diffuse, specular;
    double shininess;
    CMaterial() : ambient(0.24, 0.14, 0.07), diffuse(0.62, 0.34, 0.14),
                  specular(0.18, 0.14, 0.10), shininess(32.0) {}
};

class CLightSource {
public:
    Vec3 position, color;
    double intensity;
    CLightSource() : position(4, 6, 8), color(1, 1, 1), intensity(1.2) {}
    void SetPos(double x, double y, double z) { position = Vec3(x, y, z); }
};

class CLighting {
public:
    bool blinnPhong;  // toggle Blinn-Phong specular
    CLighting() : blinnPhong(false) {}

    unsigned int ComputeColor(const Vec3& worldPos, const Vec3& normal,
                               const Vec3& viewPos, const CMaterial& mat,
                               const CLightSource& light) {
        Vec3 N = normal.Normalize();
        Vec3 L = (light.position - worldPos).Normalize();
        Vec3 V = (viewPos - worldPos).Normalize();
        double ndotl = (std::max)(0.0, N.Dot(L));
        double spec;
        if (blinnPhong) {
            Vec3 H = (L + V).Normalize();   // half-vector
            double ndoth = (std::max)(0.0, N.Dot(H));
            spec = pow(ndoth, mat.shininess * 1.5);  // compensate exponent
        } else {
            Vec3 R = (N * (2.0 * N.Dot(L)) - L).Normalize();
            double rdotv = (std::max)(0.0, R.Dot(V));
            spec = pow(rdotv, mat.shininess);
        }
        double r = mat.ambient.x * 0.25 + mat.diffuse.x * ndotl * light.color.x + mat.specular.x * spec * light.color.x;
        double g = mat.ambient.y * 0.25 + mat.diffuse.y * ndotl * light.color.y + mat.specular.y * spec * light.color.y;
        double b = mat.ambient.z * 0.25 + mat.diffuse.z * ndotl * light.color.z + mat.specular.z * spec * light.color.z;
        r = (std::min)(1.0, r * light.intensity);
        g = (std::min)(1.0, g * light.intensity);
        b = (std::min)(1.0, b * light.intensity);
        return 0xFF000000 | ((unsigned int)(r*255) << 16) | ((unsigned int)(g*255) << 8) | (unsigned int)(b*255);
    }
};
