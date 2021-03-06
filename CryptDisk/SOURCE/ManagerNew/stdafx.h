
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include <comdef.h>
#include <initguid.h>
#include <wbemidl.h>

#include <Windows.h>
#include <WinIoCtl.h>
#include <Dbt.h>
#include <winsvc.h>

#include <ppl.h>
#include <agents.h>

// STD Lib
#include <math.h>

// STD
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <exception>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <regex>

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/range.hpp>
#include <boost/signals2.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>
#include <boost/system/system_error.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "CryptoLib.h"
#include <afxdlgs.h>

// These ones should be after STL headers
#include <Pdh.h>
#include <PdhMsg.h>

#include "../Core/scope_exit.h"

// Definitions
#define USER_MODE
#define _USER_MODE_
#define NOMINMAX

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
