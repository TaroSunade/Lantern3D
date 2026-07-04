#pragma once

class CZBuffer {
public:
    int width, height;
    double* depthBuf;
    unsigned int* colorBuf;
    CZBuffer() : width(0), height(0), depthBuf(nullptr), colorBuf(nullptr) {}
    ~CZBuffer() { Clear(); }
    void Init(int w, int h) {
        if (w <= 0 || h <= 0) { Clear(); return; }
        if (w == width && h == height && depthBuf && colorBuf) {
            Reset();
            return;
        }
        Clear(); width = w; height = h;
        depthBuf = new double[w * h];
        colorBuf = new unsigned int[w * h];
        Reset();
    }
    void Reset() {
        for (int i = 0; i < width * height; i++) {
            depthBuf[i] = 1e10;
            colorBuf[i] = 0xFF1A1A2E;
        }
    }
    void Clear() { delete[] depthBuf; depthBuf = nullptr; delete[] colorBuf; colorBuf = nullptr; width = height = 0; }
    bool TestAndSet(int x, int y, double depth, unsigned int color) {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        int idx = y * width + x;
        if (depth < depthBuf[idx]) { depthBuf[idx] = depth; colorBuf[idx] = color; return true; }
        return false;
    }
};
