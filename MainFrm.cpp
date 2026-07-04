#include "stdafx.h"
#include "Lantern3D.h"
#include "MainFrm.h"
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd) ON_WM_CREATE() END_MESSAGE_MAP()
static UINT indicators[] = { ID_SEPARATOR, ID_INDICATOR_CAPS, ID_INDICATOR_NUM, ID_INDICATOR_SCRL };
CMainFrame::CMainFrame() {} CMainFrame::~CMainFrame() {}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) { return CFrameWnd::PreCreateWindow(cs); }
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY)) return -1;
    m_wndToolBar.LoadToolBar(IDR_MAINFRAME); m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY); DockControlBar(&m_wndToolBar);
    if (!m_wndStatusBar.Create(this)) return -1;
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
    return 0;
}
