#pragma once
#ifndef __AFXWIN_H__
    #error "include stdafx.h before including this file"
#endif
#include "resource.h"
class CLantern3DApp : public CWinApp {
public: CLantern3DApp(); virtual BOOL InitInstance(); DECLARE_MESSAGE_MAP()
};
extern CLantern3DApp theApp;
