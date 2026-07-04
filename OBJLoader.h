#pragma once
#include "Vec3.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

class OBJLoader {
public:
    std::vector<Vec3> vertices, normals;
    std::vector<Vec2> texcoords;
    std::vector<Face> faces;

    struct ObjIndex {
        int v, t, n;
        ObjIndex() : v(-1), t(-1), n(-1) {}
    };

    bool Load(const char* filename) {
        vertices.clear(); normals.clear(); texcoords.clear(); faces.clear();
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            std::string type; iss >> type;
            if (type == "v") { Vec3 v; iss >> v.x >> v.y >> v.z; vertices.push_back(v); }
            else if (type == "vn") { Vec3 n; iss >> n.x >> n.y >> n.z; normals.push_back(n); }
            else if (type == "vt") { Vec2 t; iss >> t.u >> t.v; texcoords.push_back(t); }
            else if (type == "f") {
                std::vector<ObjIndex> poly;
                std::string part;
                while (iss >> part) {
                    ObjIndex idx = ParseFaceIndex(part);
                    if (idx.v >= 0 && idx.v < (int)vertices.size()) poly.push_back(idx);
                }
                for (size_t i = 1; i + 1 < poly.size(); i++) {
                    Face f;
                    SetFaceVertex(f, 0, poly[0]);
                    SetFaceVertex(f, 1, poly[i]);
                    SetFaceVertex(f, 2, poly[i + 1]);
                    faces.push_back(f);
                }
            }
        }
        file.close();
        if (normals.empty()) ComputeNormals();
        return !vertices.empty();
    }
    void ComputeNormals() {
        normals.assign(vertices.size(), Vec3(0, 0, 0));
        for (const auto& f : faces) {
            Vec3 a = vertices[f.v[0]], b = vertices[f.v[1]], c = vertices[f.v[2]];
            Vec3 n = (b - a).Cross(c - a).Normalize();
            for (int i = 0; i < 3; i++) normals[f.v[i]] = normals[f.v[i]] + n;
        }
        for (auto& n : normals) n = n.Normalize();
        for (auto& f : faces)
            for (int i = 0; i < 3; i++)
                f.n[i] = f.v[i];
    }

private:
    int ToZeroBased(const std::string& text) const {
        if (text.empty()) return -1;
        int value = atoi(text.c_str());
        return value > 0 ? value - 1 : -1;
    }

    ObjIndex ParseFaceIndex(const std::string& part) const {
        ObjIndex idx;
        size_t first = part.find('/');
        if (first == std::string::npos) {
            idx.v = ToZeroBased(part);
            return idx;
        }

        idx.v = ToZeroBased(part.substr(0, first));
        size_t second = part.find('/', first + 1);
        if (second == std::string::npos) {
            idx.t = ToZeroBased(part.substr(first + 1));
            return idx;
        }

        idx.t = ToZeroBased(part.substr(first + 1, second - first - 1));
        idx.n = ToZeroBased(part.substr(second + 1));
        return idx;
    }

    void SetFaceVertex(Face& face, int dst, const ObjIndex& src) {
        face.v[dst] = src.v;
        face.t[dst] = src.t;
        face.n[dst] = src.n;
    }
};
