#pragma once
class CMainFrame : public CFrameWnd {
protected: DECLARE_DYNCREATE(CMainFrame)
public: CMainFrame(); virtual ~CMainFrame(); virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); DECLARE_MESSAGE_MAP()
private: CStatusBar m_wndStatusBar; CToolBar m_wndToolBar;
};
