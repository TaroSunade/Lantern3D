#include "stdafx.h"
#include "Lantern3D.h"
#include "MainFrm.h"
#include "LanternDoc.h"
#include "LanternView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
BEGIN_MESSAGE_MAP(CLantern3DApp, CWinApp) ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew) END_MESSAGE_MAP()
CLantern3DApp::CLantern3DApp() {}
CLantern3DApp theApp;
BOOL CLantern3DApp::InitInstance() {
    CWinApp::InitInstance(); SetRegistryKey(_T("Lantern3D"));
    CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME,
        RUNTIME_CLASS(CLanternDoc), RUNTIME_CLASS(CMainFrame), RUNTIME_CLASS(CLanternView));
    AddDocTemplate(pDocTemplate);
    CCommandLineInfo cmdInfo; ParseCommandLine(cmdInfo);
    if (!ProcessShellCommand(cmdInfo)) return FALSE;
    m_pMainWnd->SetWindowText(_T("Realistic Rendering - Car Model"));
    m_pMainWnd->ShowWindow(m_nCmdShow); m_pMainWnd->UpdateWindow();
    return TRUE;
}
