#pragma once
class CLightSettingsDlg : public CDialogEx {
    DECLARE_DYNAMIC(CLightSettingsDlg)
public: CLightSettingsDlg(CWnd* pParent = nullptr); virtual ~CLightSettingsDlg();
    double m_lightX, m_lightY, m_lightZ, m_intensity;
    enum { IDD = IDD_LIGHT_DLG };
protected: virtual void DoDataExchange(CDataExchange* pDX); DECLARE_MESSAGE_MAP()
};
