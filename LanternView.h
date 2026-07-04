#pragma once
#include "RenderPipeline.h"

class CLanternView : public CView {
protected:
    DECLARE_DYNCREATE(CLanternView)
public:
    CRenderPipeline pipeline;
    CPen wirePen;
    CLanternView();
    virtual ~CLanternView();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnDraw(CDC* pDC);
    virtual void OnInitialUpdate();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnModeWireframe();
    afx_msg void OnModeFlat();
    afx_msg void OnModeGouraud();
    afx_msg void OnModePhong();
    afx_msg void OnModeTextured();
    afx_msg void OnUpdateModeWireframe(CCmdUI* p);
    afx_msg void OnUpdateModeFlat(CCmdUI* p);
    afx_msg void OnUpdateModeGouraud(CCmdUI* p);
    afx_msg void OnUpdateModePhong(CCmdUI* p);
    afx_msg void OnUpdateModeTextured(CCmdUI* p);
    afx_msg void OnLightSettings();
    afx_msg void OnLoadTexture();
    afx_msg void OnBlinnPhong();
    afx_msg void OnUpdateBlinnPhong(CCmdUI* p);
    afx_msg void OnDualLight();
    afx_msg void OnUpdateDualLight(CCmdUI* p);
    afx_msg void OnToggleAnimation();
    afx_msg void OnUpdateToggleAnimation(CCmdUI* p);
    DECLARE_MESSAGE_MAP()
private:
    bool SetupOpenGL(CDC* pDC);
    bool HandleModeKey(UINT nChar);
    CPoint lastMouse; bool dragging; UINT_PTR timerID; HGLRC hGLRC;
};
