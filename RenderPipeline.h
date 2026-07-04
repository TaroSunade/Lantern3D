#pragma once
#include "OBJLoader.h"
#include "Transform.h"
#include "Projection.h"
#include "ZBuffer.h"
#include "Lighting.h"
#include "Texture.h"
#include <vector>
#include <gl/GL.h>

// ============================================================
// Render Mode Enum
// ============================================================
enum RenderMode {
    RM_WIREFRAME = 0,   // draw triangle edges with GDI lines
    RM_FLAT,            // one normal per triangle (flat shading) + Phong lighting
    RM_GOURAUD,         // (same as flat for now - vertex color interpolation)
    RM_PHONG,           // interpolate normals per pixel (Phong shading) + Phong lighting
    RM_TEXTURED,        // interpolate UVs per pixel + texture sample
    RM_OPENGL,          // OpenGL fixed-function lighting path
    RM_COUNT
};

// ============================================================
// CRenderPipeline - Main Rendering Pipeline
// ============================================================
// Pipeline steps (executed every frame in Render()):
//   [1] Model Loading   -> OBJ file or built-in cube
//   [2] World Transform  -> Scale x1.5, rotate by (angleX, angleY)
//   [3] View Transform   -> LookAt camera at (0, 0, zoom) looking at origin
//   [4] Projection       -> Perspective projection (45 deg FOV, near=0.1, far=100)
//   [5] Rasterization    -> Barycentric interpolation + Z-buffer depth test
//   [6] Shading          -> Phong lighting model (ambient + diffuse + specular)
//   [7] Display          -> SetDIBitsToDevice to blit color buffer to screen
// ============================================================

class CRenderPipeline {
public:
    // --- Resources ---
    OBJLoader model;          // 3D model data (vertices, normals, texcoords, faces)
    CTransform worldMatrix;   // model-to-world transform (scale + rotation)
    CTransform viewMatrix;    // world-to-view transform (LookAt camera)
    CProjection projection;   // view-to-clip transform (perspective projection)
    CZBuffer zbuffer;         // depth buffer + color buffer for rasterization
    CMaterial material;       // surface properties (Ka, Kd, Ks, shininess)
    CLightSource light;       // light position, color, intensity
    CLighting lighting;       // Phong lighting equation calculator
    CTexture texture;         // texture image (programmatic wood grain)

    // --- State ---
    RenderMode mode;    // current rendering mode
    double angleX;      // rotation around X axis (radians)
    double angleY;      // rotation around Y axis (radians)
    double zoom;        // camera distance from origin
    bool autoRotate;    // automatic Y-axis rotation toggle
    double rotateSpeed; // auto-rotation speed (rad/sec)
    bool modelLoaded;   // whether external OBJ was successfully loaded

    // --- Advanced features ---
    double bounceTime;      // bounce animation timer
    double bounceAmp;       // bounce amplitude (0.0 = disabled)
    bool hasBmpTexture;     // true if external BMP texture loaded
    bool dualLight;         // dual-light stage lighting toggle
    CLightSource light2;    // second light (warm headlight)

    CRenderPipeline() : mode(RM_PHONG), angleX(0.2), angleY(0.0), zoom(3.5),
                        autoRotate(true), rotateSpeed(0.5), modelLoaded(false),
                        bounceTime(0.0), bounceAmp(0.08), hasBmpTexture(false),
                        dualLight(true) {
        light2.SetPos(-2, 1.0, -3.5);    // front-left warm headlight
        light2.color = Vec3(1.0, 0.85, 0.4);  // warm yellow
        light2.intensity = 1.8;
        BuildFallbackCube();
    }
        BuildFallbackCube();  // always have a renderable model
    }

    // ---------------------------------------------------------------
    // Load model from external OBJ file. Falls back to built-in cube.
    // ---------------------------------------------------------------
    bool LoadModelFile(const char* filename) {
        if (model.Load(filename)) {
            NormalizeModel();
            modelLoaded = true;
            return true;
        }
        modelLoaded = false;
        BuildFallbackCube();
        return false;
    }

    void NormalizeModel() {
        if (model.vertices.empty()) return;

        Vec3 minV = model.vertices[0], maxV = model.vertices[0];
        for (size_t i = 1; i < model.vertices.size(); i++) {
            const Vec3& v = model.vertices[i];
            if (v.x < minV.x) minV.x = v.x; if (v.x > maxV.x) maxV.x = v.x;
            if (v.y < minV.y) minV.y = v.y; if (v.y > maxV.y) maxV.y = v.y;
            if (v.z < minV.z) minV.z = v.z; if (v.z > maxV.z) maxV.z = v.z;
        }

        Vec3 center((minV.x + maxV.x) * 0.5, (minV.y + maxV.y) * 0.5, (minV.z + maxV.z) * 0.5);
        double sizeX = maxV.x - minV.x, sizeY = maxV.y - minV.y, sizeZ = maxV.z - minV.z;
        double maxSize = sizeX;
        if (sizeY > maxSize) maxSize = sizeY;
        if (sizeZ > maxSize) maxSize = sizeZ;
        double scale = maxSize > 1e-10 ? 2.0 / maxSize : 1.0;

        for (auto& v : model.vertices) v = (v - center) * scale;
    }

    // ---------------------------------------------------------------
    // Timer-driven auto-rotation
    // ---------------------------------------------------------------
    void Animate(double dt) {
        if (dt <= 0.0 || dt > 0.5) return;
        if (autoRotate) {
            angleY += rotateSpeed * dt;
            bounceTime += rotateSpeed * 3.0 * dt;  // bounce sync'd with rotation
        }
    }

    // ---------------------------------------------------------------
    // Hardcoded unit cube (12 triangles, 6 face normals, 4 texcoords)
    // Used as fallback when OBJ file is missing.
    // ---------------------------------------------------------------
    void BuildFallbackCube() {
        model.vertices.clear(); model.normals.clear(); model.texcoords.clear(); model.faces.clear();
        double s = 0.6;
        // 8 vertices of a cube centered at origin
        double v[8][3] = {{-s,-s,-s},{s,-s,-s},{-s,s,-s},{s,s,-s},{-s,-s,s},{s,-s,s},{-s,s,s},{s,s,s}};
        for (int i = 0; i < 8; i++) model.vertices.push_back(Vec3(v[i][0], v[i][1], v[i][2]));
        // 6 face normals: -Z, +Z, -Y, +Y, -X, +X
        double n[6][3] = {{0,0,-1},{0,0,1},{0,-1,0},{0,1,0},{-1,0,0},{1,0,0}};
        for (int i = 0; i < 6; i++) model.normals.push_back(Vec3(n[i][0], n[i][1], n[i][2]));
        // Simple UVs for 8 vertices
        for (int i = 0; i < 8; i++) model.texcoords.push_back(Vec2(i%2*1.0, i/2%2*1.0));
        // 12 triangles (2 per face), each with vertex indices + normal index + texcoord index
        int fc[12][3] = {{0,2,3},{0,3,1},{4,5,7},{4,7,6},{0,1,5},{0,5,4},
                         {2,6,7},{2,7,3},{0,4,6},{0,6,2},{1,3,7},{1,7,5}};
        int fn[12] = {0,0,1,1,2,2,3,3,4,4,5,5};
        for (int i = 0; i < 12; i++) {
            Face f; f.v[0]=fc[i][0]; f.v[1]=fc[i][1]; f.v[2]=fc[i][2];
            f.n[0]=f.n[1]=f.n[2]=fn[i]; f.t[0]=fc[i][0]; f.t[1]=fc[i][1]; f.t[2]=fc[i][2];
            model.faces.push_back(f);
        }
    }

    void GenerateTexture() { texture.GenerateWoodTexture(); }

    // ---------------------------------------------------------------
    // [3] View Transform - LookAt matrix
    // Camera at (0, 0, zoom) looking at origin (0, 0, 0), up = Y axis.
    // Builds orthonormal basis: forward(Z), right(X), up(Y).
    // Maps world-space coordinates to camera-space (view-space).
    // ---------------------------------------------------------------
    void SetupView() {
        viewMatrix.Identity();
        Vec3 eye(0, 0, zoom), target(0, 0, 0), up(0, 1, 0);
        Vec3 fwd = (target - eye).Normalize();       // camera forward = look direction
        Vec3 right = fwd.Cross(up).Normalize();       // camera right = fwd x up
        Vec3 newUp = right.Cross(fwd);                // correct up = right x fwd
        // Row 0: right vector + translation
        viewMatrix.m[0][0]=right.x; viewMatrix.m[0][1]=right.y; viewMatrix.m[0][2]=right.z; viewMatrix.m[0][3]=-right.Dot(eye);
        // Row 1: up vector + translation
        viewMatrix.m[1][0]=newUp.x; viewMatrix.m[1][1]=newUp.y; viewMatrix.m[1][2]=newUp.z; viewMatrix.m[1][3]=-newUp.Dot(eye);
        // Row 2: backward vector (camera looks down -Z in view space) + translation
        viewMatrix.m[2][0]=-fwd.x; viewMatrix.m[2][1]=-fwd.y; viewMatrix.m[2][2]=-fwd.z; viewMatrix.m[2][3]=fwd.Dot(eye);
    }

    // ---------------------------------------------------------------
    // [2] World Transform - Scale x1.5 then rotate by (angleX, angleY)
    // Applied to each vertex: worldPos = RotateY * RotateX * Scale * modelPos
    // ---------------------------------------------------------------
    void SetupWorld() {
        worldMatrix.Identity();
        // Composite animation: bounce (translation) + scale + rotation
        if (autoRotate && bounceAmp > 0.0)
            worldMatrix = CTransform::Multiply(worldMatrix, CTransform::Translate(0, sin(bounceTime) * bounceAmp, 0));
        worldMatrix = CTransform::Multiply(worldMatrix, CTransform::Scale(1.5, 1.5, 1.5));
        worldMatrix = CTransform::Multiply(worldMatrix, CTransform::RotateX(angleX));
        worldMatrix = CTransform::Multiply(worldMatrix, CTransform::RotateY(angleY));
    }

    // ---------------------------------------------------------------
    // Main render entry point - called from OnDraw every frame
    // [1] Check model not empty
    // [2-4] Setup transforms + projection
    // [5-6] Transform vertices once, then draw/rasterize each face
    // [7] Flush color buffer to screen (for filled modes)
    // ---------------------------------------------------------------
    void Render(CDC* pDC, int scrW, int scrH) {
        if (model.faces.empty() || scrW <= 0 || scrH <= 0) return;

        // Initialize Z-buffer with background color
        zbuffer.Init(scrW, scrH);

        // Build transform matrices
        SetupView();    // view matrix (step 3)
        SetupWorld();   // world matrix (step 2)

        // Build perspective projection matrix (step 4)
        projection.SetPerspective(45.0, (double)scrW / (double)scrH, 0.1, 100.0);

        // Pre-compute ModelView matrix = View * World (applied to each vertex)
        CTransform mv = CTransform::Multiply(viewMatrix, worldMatrix);

        // Keep lighting inputs in view space, matching transformed vertices.
        CLightSource viewLight = light;
        viewLight.position = viewMatrix.TransformPoint(light.position);
        CLightSource viewLight2 = light2;
        viewLight2.position = viewMatrix.TransformPoint(light2.position);

        std::vector<Vec3> viewVertices(model.vertices.size());
        for (size_t i = 0; i < model.vertices.size(); i++)
            viewVertices[i] = mv.TransformPoint(model.vertices[i]);

        std::vector<Vec3> viewNormals(model.normals.size());
        for (size_t i = 0; i < model.normals.size(); i++)
            viewNormals[i] = mv.TransformNormal(model.normals[i]);

        // Process each triangle face
        for (size_t fi = 0; fi < model.faces.size(); fi++) {
            const Face& face = model.faces[fi];
            if (face.v[0] < 0 || face.v[1] < 0 || face.v[2] < 0) continue;
            if (face.v[0] >= (int)viewVertices.size() || face.v[1] >= (int)viewVertices.size() || face.v[2] >= (int)viewVertices.size()) continue;

            // Arrays for view-space data
            Vec3 wv[3];      // view-space vertex positions
            Vec3 wn[3];      // view-space normals
            Vec2 tv[3];      // texture coordinates (unchanged by transform)
            bool hasN = (!viewNormals.empty() && face.n[0]>=0 && face.n[1]>=0 && face.n[2]>=0 &&
                         face.n[0] < (int)viewNormals.size() && face.n[1] < (int)viewNormals.size() && face.n[2] < (int)viewNormals.size());
            bool hasT = (!model.texcoords.empty() && face.t[0]>=0 && face.t[1]>=0 && face.t[2]>=0 &&
                         face.t[0] < (int)model.texcoords.size() && face.t[1] < (int)model.texcoords.size() && face.t[2] < (int)model.texcoords.size());

            // Transform each vertex from model-space to view-space
            for (int i = 0; i < 3; i++) {
                wv[i] = viewVertices[face.v[i]];             // position
                if (hasN) wn[i] = viewNormals[face.n[i]];    // normal
                if (hasT) tv[i] = model.texcoords[face.t[i]]; // UV
                else if (mode == RM_TEXTURED) tv[i] = GenerateWoodUV(model.vertices[face.v[i]]);
            }
            if (mode == RM_TEXTURED && !hasT) hasT = true;

            // Branch by render mode
            if (mode == RM_WIREFRAME) {
                // Wireframe: project vertices to screen, draw lines with GDI
                DrawWireframe(pDC, wv, scrW, scrH);
            } else {
                // Filled modes: rasterize triangle to Z-buffer with per-pixel shading
                Rasterize(pDC, fi, wv, wn, tv, hasN, hasT, viewLight, viewLight2, scrW, scrH);
            }
        }
    }

    void RenderOpenGL(int scrW, int scrH) {
        if (model.faces.empty() || scrW <= 0 || scrH <= 0) return;

        glViewport(0, 0, scrW, scrH);
        glClearColor(20.0f / 255.0f, 20.0f / 255.0f, 35.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_NORMALIZE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        double nearZ = 0.1, farZ = 100.0;
        double top = nearZ * tan(45.0 * 3.141592653589793 / 360.0);
        double right = top * (double)scrW / (double)scrH;
        glFrustum(-right, right, -top, top, nearZ, farZ);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslated(0.0, 0.0, -zoom);

        if (autoRotate && bounceAmp > 0.0)
            glTranslated(0, sin(bounceTime) * bounceAmp, 0);

        GLfloat lightPos[] = { (GLfloat)light.position.x, (GLfloat)light.position.y, (GLfloat)light.position.z, 1.0f };
        GLfloat lightColor[] = { (GLfloat)light.color.x, (GLfloat)light.color.y, (GLfloat)light.color.z, 1.0f };
        GLfloat ambient[] = { 0.24f, 0.14f, 0.07f, 1.0f };
        GLfloat diffuse[] = { 0.62f, 0.34f, 0.14f, 1.0f };
        GLfloat specular[] = { 0.18f, 0.14f, 0.10f, 1.0f };
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, (GLfloat)material.shininess);

        glRotated(angleY * 180.0 / 3.141592653589793, 0.0, 1.0, 0.0);
        glRotated(angleX * 180.0 / 3.141592653589793, 1.0, 0.0, 0.0);
        glScaled(1.5, 1.5, 1.5);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLES);
        for (size_t fi = 0; fi < model.faces.size(); fi++) {
            const Face& face = model.faces[fi];
            if (face.v[0] < 0 || face.v[1] < 0 || face.v[2] < 0) continue;
            if (face.v[0] >= (int)model.vertices.size() || face.v[1] >= (int)model.vertices.size() || face.v[2] >= (int)model.vertices.size()) continue;

            bool hasN = (!model.normals.empty() && face.n[0] >= 0 && face.n[1] >= 0 && face.n[2] >= 0 &&
                         face.n[0] < (int)model.normals.size() && face.n[1] < (int)model.normals.size() && face.n[2] < (int)model.normals.size());
            Vec3 faceNormal;
            if (!hasN) {
                Vec3 a = model.vertices[face.v[0]], b = model.vertices[face.v[1]], c = model.vertices[face.v[2]];
                faceNormal = (b - a).Cross(c - a).Normalize();
            }

            for (int i = 0; i < 3; i++) {
                const Vec3& n = hasN ? model.normals[face.n[i]] : faceNormal;
                const Vec3& v = model.vertices[face.v[i]];
                glNormal3d(n.x, n.y, n.z);
                glVertex3d(v.x, v.y, v.z);
            }
        }
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_LIGHTING);
    }

private:
    Vec2 GenerateWoodUV(const Vec3& v) const {
        return Vec2(v.x * 1.6 + v.z * 0.18, v.y * 1.1 + v.z * 0.12);
    }

    unsigned int ModulateColor(unsigned int base, unsigned int lightColor) const {
        int br = (base >> 16) & 255, bg = (base >> 8) & 255, bb = base & 255;
        int lr = (lightColor >> 16) & 255, lg = (lightColor >> 8) & 255, lb = lightColor & 255;
        int r = br * (80 + lr) / 335;
        int g = bg * (80 + lg) / 335;
        int b = bb * (80 + lb) / 335;
        return 0xFF000000 | (r << 16) | (g << 8) | b;
    }

    // ---------------------------------------------------------------
    // Wireframe rendering: project 3 vertices to screen, draw 3 edges
    // Uses GDI MoveTo/LineTo (simple, no Z-buffer needed for wireframe)
    // ---------------------------------------------------------------
    void DrawWireframe(CDC* pDC, Vec3 wv[3], int sw, int sh) {
        int sx[3], sy[3]; double dz;
        for (int i = 0; i < 3; i++) {
            projection.Project(wv[i].x, wv[i].y, wv[i].z, sx[i], sy[i], dz, sw, sh);
        }
        for (int e = 0; e < 3; e++) {
            pDC->MoveTo(sx[e], sy[e]);
            pDC->LineTo(sx[(e+1)%3], sy[(e+1)%3]);
        }
    }

    // ---------------------------------------------------------------
    // [5] Rasterization + [6] Shading (scanline triangle fill)
    //
    // Algorithm:
    //   1. Project 3 vertices to screen coordinates (sx[i], sy[i]) + depth d[i]
    //   2. Compute bounding box of the triangle in screen space
    //   3. For each pixel (x,y) in bounding box:
    //      a. Compute barycentric coordinates (b0, b1, b2)
    //      b. If all >= 0, pixel is inside triangle
    //      c. Interpolate depth: depth = b0*d0 + b1*d1 + b2*d2
    //      d. Compute color based on render mode:
    //         - TEXTURED: interpolate UV, sample texture
    //         - PHONG:    interpolate normal, compute Phong lighting per pixel
    //         - FLAT:     use face normal, compute Phong lighting once per triangle
    //      e. Z-buffer test: if depth < zbuffer[x][y], write color + update depth
    // ---------------------------------------------------------------
    void Rasterize(CDC*, size_t fi, Vec3 wv[3], Vec3 wn[3], Vec2 tv[3],
                   bool hasN, bool hasT, const CLightSource& viewLight,
                   const CLightSource& viewLight2, int sw, int sh) {

        // Step 1: Project vertices to screen
        int sx[3], sy[3]; double d[3];
        for (int i = 0; i < 3; i++)
            projection.Project(wv[i].x, wv[i].y, wv[i].z, sx[i], sy[i], d[i], sw, sh);

        // Step 2: Bounding box (clamped to screen)
        int minX = sx[0], maxX = sx[0], minY = sy[0], maxY = sy[0];
        for (int i = 1; i < 3; i++) {
            if (sx[i] < minX) minX = sx[i]; if (sx[i] > maxX) maxX = sx[i];
            if (sy[i] < minY) minY = sy[i]; if (sy[i] > maxY) maxY = sy[i];
        }
        if (minX < 0) minX = 0; if (maxX >= sw) maxX = sw - 1;
        if (minY < 0) minY = 0; if (maxY >= sh) maxY = sh - 1;
        if (minX > maxX || minY > maxY) return;

        // Triangle area for barycentric denominator (2x area, signed)
        double triArea = (double)(sx[1]-sx[0])*(sy[2]-sy[0]) - (double)(sx[2]-sx[0])*(sy[1]-sy[0]);
        if (fabs(triArea) < 0.5) return;   // degenerate triangle (too small)
        double invA = 1.0 / triArea;

        Vec3 camPos(0, 0, 0);  // camera position in view space

        // Step 3: Scanline loop
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                // a. Barycentric coordinates (b0,b1,b2) relative to 3 edges
                double b0 = ((sx[1]-x)*(sy[2]-y) - (sx[2]-x)*(sy[1]-y)) * invA;
                double b1 = ((sx[2]-x)*(sy[0]-y) - (sx[0]-x)*(sy[2]-y)) * invA;
                double b2 = 1.0 - b0 - b1;

                // b. Inside test
                if (b0 < -1e-6 || b1 < -1e-6 || b2 < -1e-6) continue;

                // c. Perspective-correct depth interpolation
                double depth = b0*d[0] + b1*d[1] + b2*d[2];
                unsigned int color;

                // d. Per-pixel shading
                auto lightColor = [&](const Vec3& wp, const Vec3& n) -> unsigned int {
                    if (dualLight) return lighting.ComputeDualLight(wp, n, camPos, material, viewLight, viewLight2);
                    else           return lighting.ComputeColor(wp, n, camPos, material, viewLight);
                };
                if (mode == RM_TEXTURED && hasT) {
                    // Texture mapping: interpolate UV, sample texture image
                    double tu = b0*tv[0].u + b1*tv[1].u + b2*tv[2].u;
                    double tv2 = b0*tv[0].v + b1*tv[1].v + b2*tv[2].v;
                    Vec3 n;
                    if (hasN) n = (wn[0]*b0 + wn[1]*b1 + wn[2]*b2).Normalize();
                    else n = (wv[1]-wv[0]).Cross(wv[2]-wv[0]).Normalize();
                    Vec3 wp = wv[0]*b0 + wv[1]*b1 + wv[2]*b2;
                    color = ModulateColor(texture.Sample(tu, tv2), lightColor(wp, n));
                }
                else if ((mode == RM_PHONG || mode == RM_TEXTURED) && hasN) {
                    // Phong shading: interpolate normal per-pixel, compute Phong lighting
                    Vec3 n = (wn[0]*b0 + wn[1]*b1 + wn[2]*b2).Normalize();
                    Vec3 wp = wv[0]*b0 + wv[1]*b1 + wv[2]*b2;
                    color = lightColor(wp, n);
                }
                else {
                    // Flat shading: use face normal, compute Phong lighting
                    Vec3 n;
                    if (hasN) n = wn[0];
                    else n = (wv[1]-wv[0]).Cross(wv[2]-wv[0]).Normalize();
                    Vec3 wp = wv[0]*b0 + wv[1]*b1 + wv[2]*b2;
                    color = lightColor(wp, n);
                }

                // e. Z-buffer depth test + write
                zbuffer.TestAndSet(x, y, depth, color);
            }
        }
    }

public:
    // ---------------------------------------------------------------
    // [7] Display - Copy Z-buffer color data to screen via GDI
    // Uses SetDIBitsToDevice for fast 32-bit blit.
    // ---------------------------------------------------------------
    void Flush(CDC* pDC) {
        if (!zbuffer.colorBuf || zbuffer.width < 1 || zbuffer.height < 1) return;
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = zbuffer.width;
        bi.bmiHeader.biHeight = -zbuffer.height;  // negative = top-down DIB
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;             // ARGB format
        bi.bmiHeader.biCompression = BI_RGB;
        SetDIBitsToDevice(pDC->m_hDC, 0, 0, zbuffer.width, zbuffer.height,
            0, 0, 0, zbuffer.height, zbuffer.colorBuf, &bi, DIB_RGB_COLORS);
    }
};
