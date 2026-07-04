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
            spec = pow(ndoth, mat.shininess * 1.5);
        } else {
            Vec3 R = (N * (2.0 * N.Dot(L)) - L).Normalize();
            double rdotv = (std::max)(0.0, R.Dot(V));
            spec = pow(rdotv, mat.shininess);
        }
        // Distance attenuation: 1 / (1 + dist² * k)
        double dist = (light.position - worldPos).Length();
        double atten = 1.0 / (1.0 + dist * dist * 0.02);

        double r = mat.ambient.x * 0.25 + (mat.diffuse.x * ndotl + mat.specular.x * spec)
                   * light.color.x * atten;
        double g = mat.ambient.y * 0.25 + (mat.diffuse.y * ndotl + mat.specular.y * spec)
                   * light.color.y * atten;
        double b = mat.ambient.z * 0.25 + (mat.diffuse.z * ndotl + mat.specular.z * spec)
                   * light.color.z * atten;
        r = (std::min)(1.0, r * light.intensity);
        g = (std::min)(1.0, g * light.intensity);
        b = (std::min)(1.0, b * light.intensity);
        return 0xFF000000 | ((unsigned int)(r*255) << 16) | ((unsigned int)(g*255) << 8) | (unsigned int)(b*255);
    }

    // Dual-light: ambient computed once, diffuse+specular from two sources added
    unsigned int ComputeDualLight(const Vec3& wp, const Vec3& normal,
                                   const Vec3& viewPos, const CMaterial& mat,
                                   const CLightSource& light1,
                                   const CLightSource& light2) {
        Vec3 N = normal.Normalize();
        Vec3 V = (viewPos - wp).Normalize();
        Vec3 amb(mat.ambient.x * 0.25, mat.ambient.y * 0.25, mat.ambient.z * 0.25);

        auto calc = [&](const CLightSource& lt) -> Vec3 {
            Vec3 L = (lt.position - wp).Normalize();
            double ndotl = (std::max)(0.0, N.Dot(L));
            double spec;
            if (blinnPhong) {
                Vec3 H = (L + V).Normalize();
                spec = pow((std::max)(0.0, N.Dot(H)), mat.shininess * 1.5);
            } else {
                Vec3 R = (N * (2.0 * N.Dot(L)) - L).Normalize();
                spec = pow((std::max)(0.0, R.Dot(V)), mat.shininess);
            }
            double dist = (lt.position - wp).Length();
            double atten = 1.0 / (1.0 + dist * dist * 0.02);
            return Vec3(
                (mat.diffuse.x * ndotl + mat.specular.x * spec) * lt.color.x * atten,
                (mat.diffuse.y * ndotl + mat.specular.y * spec) * lt.color.y * atten,
                (mat.diffuse.z * ndotl + mat.specular.z * spec) * lt.color.z * atten
            ) * lt.intensity;
        };

        Vec3 d1 = calc(light1);
        Vec3 d2 = calc(light2);
        double r = (std::min)(1.0, amb.x + d1.x + d2.x);
        double g = (std::min)(1.0, amb.y + d1.y + d2.y);
        double b = (std::min)(1.0, amb.z + d1.z + d2.z);
        return 0xFF000000 | ((unsigned int)(r*255)<<16) | ((unsigned int)(g*255)<<8) | (unsigned int)(b*255);
    }
};
