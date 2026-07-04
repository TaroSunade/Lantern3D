#include "stdafx.h"
#include "Lantern3D.h"
#include "LightSettingsDlg.h"
IMPLEMENT_DYNAMIC(CLightSettingsDlg, CDialogEx)
CLightSettingsDlg::CLightSettingsDlg(CWnd* pParent) : CDialogEx(IDD_LIGHT_DLG, pParent), m_lightX(4), m_lightY(6), m_lightZ(8), m_intensity(1.2) {}
CLightSettingsDlg::~CLightSettingsDlg() {}
void CLightSettingsDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_LIGHT_X, m_lightX); DDX_Text(pDX, IDC_LIGHT_Y, m_lightY);
    DDX_Text(pDX, IDC_LIGHT_Z, m_lightZ); DDX_Text(pDX, IDC_INTENSITY, m_intensity);
}
BEGIN_MESSAGE_MAP(CLightSettingsDlg, CDialogEx) END_MESSAGE_MAP()
