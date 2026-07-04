#include "stdafx.h"
#include "Lantern3D.h"
#include "LanternDoc.h"
IMPLEMENT_DYNCREATE(CLanternDoc, CDocument)
BEGIN_MESSAGE_MAP(CLanternDoc, CDocument) END_MESSAGE_MAP()

CLanternDoc::CLanternDoc() {
    TCHAR exePath[MAX_PATH]; GetModuleFileName(NULL, exePath, MAX_PATH);
    CString dir = exePath; dir = dir.Left(dir.ReverseFind('\\') + 1);

    // Try car model first
    m_modelPath = dir + _T("car.obj");
    if (GetFileAttributes(m_modelPath) != INVALID_FILE_ATTRIBUTES) return;

    // Try test cube
    m_modelPath = dir + _T("test_cube.obj");
    if (GetFileAttributes(m_modelPath) != INVALID_FILE_ATTRIBUTES) return;

    // Try project root
    dir = dir.Left(dir.ReverseFind('\\'));
    dir = dir.Left(dir.ReverseFind('\\') + 1);
    m_modelPath = dir + _T("test_cube.obj");
}
CLanternDoc::~CLanternDoc() {}
BOOL CLanternDoc::OnNewDocument() { return CDocument::OnNewDocument(); }
void CLanternDoc::Serialize(CArchive& ar) {}
