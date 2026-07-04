#pragma once
#include <windows.h>
#include <cmath>

class CTexture {
public:
    int width, height;
    unsigned int* pixels;
    CTexture() : width(0), height(0), pixels(nullptr) {}
    ~CTexture() { Clear(); }
    void Clear() { delete[] pixels; pixels = nullptr; width = height = 0; }

    bool LoadBMP(const char* filename) {
        Clear();
        HBITMAP hBmp = (HBITMAP)LoadImageA(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (!hBmp) return false;
        BITMAP bm; GetObject(hBmp, sizeof(BITMAP), &bm);
        width = bm.bmWidth; height = bm.bmHeight;
        pixels = new unsigned int[width * height];
        HDC hdc = CreateCompatibleDC(NULL);
        HBITMAP old = (HBITMAP)SelectObject(hdc, hBmp);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = width; bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        GetDIBits(hdc, hBmp, 0, height, pixels, &bi, DIB_RGB_COLORS);
        SelectObject(hdc, old); DeleteDC(hdc); DeleteObject(hBmp);
        return true;
    }

    void GenerateWoodTexture() {
        Clear();
        width = 256; height = 256;
        pixels = new unsigned int[width * height];
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double u = (double)x / width;
                double v = (double)y / height;
                double grain = sin((u * 28.0 + sin(v * 18.0) * 0.45) * 3.141592653589793) * 0.5 + 0.5;
                double rings = sin((v * 9.0 + sin(u * 12.0) * 0.20) * 3.141592653589793) * 0.5 + 0.5;
                double fine = sin((u * 95.0 + v * 16.0) * 3.141592653589793) * 0.5 + 0.5;
                double tone = 0.48 + grain * 0.32 + rings * 0.16 + fine * 0.04;
                int r = (int)(115 + tone * 110);
                int g = (int)(58 + tone * 62);
                int b = (int)(24 + tone * 30);
                if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255;
                pixels[y * width + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }
    }

    unsigned int Sample(double u, double v) const {
        if (!pixels) return 0xFFFF0000;
        u = u - floor(u); v = v - floor(v);
        int px = (int)(u * (width - 1) + 0.5);
        int py = (int)(v * (height - 1) + 0.5);
        if (px < 0) px = 0; if (px >= width) px = width - 1;
        if (py < 0) py = 0; if (py >= height) py = height - 1;
        return pixels[py * width + px];
    }
};
