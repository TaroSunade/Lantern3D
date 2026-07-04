#pragma once
#include "targetver.h"
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// MFC core headers
#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <afxdialogex.h>
#include <afxcmn.h>
#include <afxcontrolbars.h>

// Standard library
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstring>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
