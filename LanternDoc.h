#pragma once
class CLanternDoc : public CDocument {
protected: DECLARE_DYNCREATE(CLanternDoc)
public: CLanternDoc(); virtual ~CLanternDoc();
    CString GetModelPath() const { return m_modelPath; }
    virtual BOOL OnNewDocument(); virtual void Serialize(CArchive& ar);
    DECLARE_MESSAGE_MAP()
private: CString m_modelPath;
};
