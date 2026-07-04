#pragma once
#include <cmath>

struct Vec3 {
    double x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(double a, double b, double c) : x(a), y(b), z(c) {}
    Vec3 operator+(const Vec3& o) const { return Vec3(x+o.x, y+o.y, z+o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x-o.x, y-o.y, z-o.z); }
    Vec3 operator*(double s) const { return Vec3(x*s, y*s, z*s); }
    double Dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
    Vec3 Cross(const Vec3& o) const {
        return Vec3(y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x);
    }
    Vec3 Normalize() const {
        double len = sqrt(x*x + y*y + z*z);
        if (len < 1e-10) return Vec3(0, 0, 0);
        return Vec3(x/len, y/len, z/len);
    }
    double Length() const { return sqrt(x*x + y*y + z*z); }
};

struct Vec2 { double u, v; Vec2() : u(0), v(0) {} Vec2(double a, double b) : u(a), v(b) {} };

struct Face { int v[3], n[3], t[3]; Face() { v[0]=v[1]=v[2]=n[0]=n[1]=n[2]=t[0]=t[1]=t[2]=-1; } };
