#include "stdafx.h"
#include "Lantern3D.h"
#include "LanternDoc.h"
#include "LanternView.h"
#include "LightSettingsDlg.h"
#include <gl/GL.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLanternView, CView)

BEGIN_MESSAGE_MAP(CLanternView, CView)
    ON_WM_TIMER() ON_WM_DESTROY() ON_WM_ERASEBKGND() ON_WM_SIZE() ON_WM_KEYDOWN() ON_WM_LBUTTONDOWN() ON_WM_MOUSEMOVE() ON_WM_RBUTTONDOWN() ON_WM_MOUSEWHEEL()
    ON_COMMAND(ID_MODE_WIREFRAME, &CLanternView::OnModeWireframe)
    ON_COMMAND(ID_MODE_FLAT, &CLanternView::OnModeFlat)
    ON_COMMAND(ID_MODE_GOURAUD, &CLanternView::OnModeGouraud)
    ON_COMMAND(ID_MODE_PHONG, &CLanternView::OnModePhong)
    ON_COMMAND(ID_MODE_TEXTURED, &CLanternView::OnModeTextured)
    ON_UPDATE_COMMAND_UI(ID_MODE_WIREFRAME, &CLanternView::OnUpdateModeWireframe)
    ON_UPDATE_COMMAND_UI(ID_MODE_FLAT, &CLanternView::OnUpdateModeFlat)
    ON_UPDATE_COMMAND_UI(ID_MODE_GOURAUD, &CLanternView::OnUpdateModeGouraud)
    ON_UPDATE_COMMAND_UI(ID_MODE_PHONG, &CLanternView::OnUpdateModePhong)
    ON_UPDATE_COMMAND_UI(ID_MODE_TEXTURED, &CLanternView::OnUpdateModeTextured)
    ON_COMMAND(ID_LIGHT_SETTINGS, &CLanternView::OnLightSettings)
    ON_COMMAND(ID_LOAD_TEXTURE, &CLanternView::OnLoadTexture)
    ON_COMMAND(ID_BLINN_PHONG, &CLanternView::OnBlinnPhong)
    ON_UPDATE_COMMAND_UI(ID_BLINN_PHONG, &CLanternView::OnUpdateBlinnPhong)
    ON_COMMAND(ID_DUAL_LIGHT, &CLanternView::OnDualLight)
    ON_UPDATE_COMMAND_UI(ID_DUAL_LIGHT, &CLanternView::OnUpdateDualLight)
    ON_COMMAND(ID_ANIMATION_TOGGLE, &CLanternView::OnToggleAnimation)
    ON_UPDATE_COMMAND_UI(ID_ANIMATION_TOGGLE, &CLanternView::OnUpdateToggleAnimation)
END_MESSAGE_MAP()

CLanternView::CLanternView() : dragging(false), timerID(0), hGLRC(NULL) { wirePen.CreatePen(PS_SOLID, 1, RGB(255, 200, 50)); }
CLanternView::~CLanternView() {
    if (timerID && ::IsWindow(m_hWnd)) KillTimer(timerID);
    timerID = 0;
}

BOOL CLanternView::PreTranslateMessage(MSG* pMsg) {
    if (pMsg->message == WM_KEYDOWN && HandleModeKey((UINT)pMsg->wParam)) return TRUE;
    return CView::PreTranslateMessage(pMsg);
}

void CLanternView::OnInitialUpdate() {
    CView::OnInitialUpdate();
    CLanternDoc* pDoc = (CLanternDoc*)GetDocument();
    CT2CA conv(pDoc->GetModelPath());
    bool ok = pipeline.LoadModelFile(conv);
    if (!ok) {
        CString msg; msg.Format(_T("Cannot load: %s\nUsing built-in cube instead."), pDoc->GetModelPath());
        AfxMessageBox(msg);
    }
    pipeline.GenerateTexture();
    pipeline.mode = RM_PHONG;
    pipeline.autoRotate = true;   // start with auto-rotation on
    timerID = SetTimer(1, 30, NULL);  // ~33 FPS animation
    SetFocus();
}

void CLanternView::OnDraw(CDC* pDC) {
    CRect rect; GetClientRect(&rect);
    CDC* drawDC = pDC;
    CDC memDC;
    CBitmap memBitmap;
    CBitmap* oldBitmap = NULL;

    if (pipeline.mode == RM_OPENGL) {
        if (SetupOpenGL(pDC)) {
            pipeline.RenderOpenGL(rect.Width(), rect.Height());
            SwapBuffers(pDC->m_hDC);
        } else {
            pDC->FillSolidRect(&rect, RGB(20, 20, 35));
        }
    } else {
        if (memDC.CreateCompatibleDC(pDC) && memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height())) {
            oldBitmap = memDC.SelectObject(&memBitmap);
            drawDC = &memDC;
        }

        drawDC->FillSolidRect(&rect, RGB(20, 20, 35));
        CPen* oldPen = drawDC->SelectObject(&wirePen);
        pipeline.Render(drawDC, rect.Width(), rect.Height());
        if (pipeline.mode != RM_WIREFRAME) pipeline.Flush(drawDC);
        drawDC->SelectObject(oldPen);
    }

    const TCHAR* names[] = { _T("Wireframe"), _T("Flat"), _T("Gouraud"), _T("Phong"), _T("Material"), _T("OpenGL") };
    CString info;
    info.Format(_T("Mode: %s | %s | Light(%.1f,%.1f,%.1f) | RX:%.0f RY:%.0f | Zoom:%.1f | %s | Model: %s"),
        names[pipeline.mode],
        pipeline.lighting.blinnPhong ? _T("Blinn-Phong") : _T("Phong"),
        pipeline.light.position.x, pipeline.light.position.y, pipeline.light.position.z,
        pipeline.angleX * 180 / 3.14159, pipeline.angleY * 180 / 3.14159,
        pipeline.zoom,
        pipeline.autoRotate ? _T("[AUTO]") : _T("[MANUAL]"),
        pipeline.modelLoaded ? _T("OBJ") : _T("CUBE"));
    drawDC->SetTextColor(RGB(200, 200, 200)); drawDC->SetBkMode(TRANSPARENT);
    drawDC->TextOut(10, rect.Height() - 25, info);

    if (drawDC == &memDC) {
        pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
        memDC.SelectObject(oldBitmap);
    }
}

void CLanternView::OnTimer(UINT_PTR nIDEvent) {
    if (nIDEvent == 1) {
        pipeline.Animate(0.03);   // ~30ms per frame
        Invalidate(FALSE);
    }
    CView::OnTimer(nIDEvent);
}
void CLanternView::OnDestroy() {
    if (timerID) { KillTimer(timerID); timerID = 0; }
    if (::GetCapture() == m_hWnd) ReleaseCapture();
    if (hGLRC) {
        if (wglGetCurrentContext() == hGLRC) wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hGLRC);
        hGLRC = NULL;
    }
    CView::OnDestroy();
}
BOOL CLanternView::OnEraseBkgnd(CDC*) { return TRUE; }
void CLanternView::OnSize(UINT nType, int cx, int cy) { CView::OnSize(nType, cx, cy); Invalidate(FALSE); }
void CLanternView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    if (!HandleModeKey(nChar)) CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CLanternView::OnLButtonDown(UINT nFlags, CPoint point) { dragging=true; lastMouse=point; SetCapture(); CView::OnLButtonDown(nFlags, point); }
void CLanternView::OnMouseMove(UINT nFlags, CPoint point) {
    if (dragging && (nFlags & MK_LBUTTON)) {
        pipeline.angleY += (point.x-lastMouse.x)*0.01; pipeline.angleX += (point.y-lastMouse.y)*0.01;
        lastMouse=point; Invalidate(FALSE);
    }
    CView::OnMouseMove(nFlags, point);
}
void CLanternView::OnRButtonDown(UINT nFlags, CPoint point) {
    dragging=false;
    if (::GetCapture() == m_hWnd) ReleaseCapture();
    CView::OnRButtonDown(nFlags, point);
}
BOOL CLanternView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
    pipeline.zoom -= zDelta * 0.001; if (pipeline.zoom < 1.5) pipeline.zoom = 1.5; if (pipeline.zoom > 15) pipeline.zoom = 15;
    Invalidate(FALSE); return CView::OnMouseWheel(nFlags, zDelta, pt);
}
void CLanternView::OnModeWireframe() { pipeline.mode=RM_WIREFRAME; Invalidate(FALSE); }
void CLanternView::OnModeFlat()      { pipeline.mode=RM_FLAT;      Invalidate(FALSE); }
void CLanternView::OnModeGouraud()  { pipeline.mode=RM_GOURAUD;  Invalidate(FALSE); }
void CLanternView::OnModePhong()    { pipeline.mode=RM_PHONG;    Invalidate(FALSE); }
void CLanternView::OnModeTextured() { pipeline.mode=RM_TEXTURED; Invalidate(FALSE); }
void CLanternView::OnUpdateModeWireframe(CCmdUI* p) { p->SetCheck(pipeline.mode==RM_WIREFRAME); }
void CLanternView::OnUpdateModeFlat(CCmdUI* p)      { p->SetCheck(pipeline.mode==RM_FLAT); }
void CLanternView::OnUpdateModeGouraud(CCmdUI* p)  { p->SetCheck(pipeline.mode==RM_GOURAUD); }
void CLanternView::OnUpdateModePhong(CCmdUI* p)    { p->SetCheck(pipeline.mode==RM_PHONG); }
void CLanternView::OnUpdateModeTextured(CCmdUI* p) { p->SetCheck(pipeline.mode==RM_TEXTURED); }
void CLanternView::OnLightSettings() {
    CLightSettingsDlg dlg; dlg.m_lightX=pipeline.light.position.x; dlg.m_lightY=pipeline.light.position.y;
    dlg.m_lightZ=pipeline.light.position.z; dlg.m_intensity=pipeline.light.intensity;
    if (dlg.DoModal()==IDOK) { pipeline.light.SetPos(dlg.m_lightX,dlg.m_lightY,dlg.m_lightZ); pipeline.light.intensity=dlg.m_intensity; Invalidate(FALSE); }
}

void CLanternView::OnLoadTexture() {
    CFileDialog dlg(TRUE, _T("bmp"), NULL, OFN_FILEMUSTEXIST, _T("BMP Files (*.bmp)|*.bmp||"), this);
    if (dlg.DoModal() == IDOK) {
        CT2CA conv(dlg.GetPathName());
        if (pipeline.texture.LoadBMP(conv)) {
            pipeline.hasBmpTexture = true;
            pipeline.mode = RM_TEXTURED;
            Invalidate(FALSE);
        } else {
            AfxMessageBox(_T("Failed to load BMP texture."));
        }
    }
}

void CLanternView::OnBlinnPhong() { pipeline.lighting.blinnPhong = !pipeline.lighting.blinnPhong; Invalidate(FALSE); }
void CLanternView::OnUpdateBlinnPhong(CCmdUI* p) { p->SetCheck(pipeline.lighting.blinnPhong); }
void CLanternView::OnDualLight() { pipeline.dualLight = !pipeline.dualLight; Invalidate(FALSE); }
void CLanternView::OnUpdateDualLight(CCmdUI* p) { p->SetCheck(pipeline.dualLight); }
void CLanternView::OnToggleAnimation() { pipeline.autoRotate = !pipeline.autoRotate; Invalidate(FALSE); }
void CLanternView::OnUpdateToggleAnimation(CCmdUI* p) { p->SetCheck(pipeline.autoRotate); }

bool CLanternView::SetupOpenGL(CDC* pDC) {
    if (!pDC || !pDC->m_hDC) return false;
    if (hGLRC) return wglMakeCurrent(pDC->m_hDC, hGLRC) == TRUE;

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int format = GetPixelFormat(pDC->m_hDC);
    if (format == 0) {
        format = ChoosePixelFormat(pDC->m_hDC, &pfd);
        if (format == 0 || !SetPixelFormat(pDC->m_hDC, format, &pfd)) return false;
    }

    hGLRC = wglCreateContext(pDC->m_hDC);
    if (!hGLRC) return false;
    return wglMakeCurrent(pDC->m_hDC, hGLRC) == TRUE;
}

bool CLanternView::HandleModeKey(UINT nChar) {
    if (nChar == '1' || nChar == VK_NUMPAD1) pipeline.mode = RM_WIREFRAME;
    else if (nChar == '2' || nChar == VK_NUMPAD2) pipeline.mode = RM_FLAT;
    else if (nChar == '3' || nChar == VK_NUMPAD3) pipeline.mode = RM_PHONG;
    else if (nChar == '4' || nChar == VK_NUMPAD4) pipeline.mode = RM_TEXTURED;
    else if (nChar == '5' || nChar == VK_NUMPAD5) pipeline.mode = RM_OPENGL;
    else return false;
    Invalidate(FALSE);
    return true;
}
