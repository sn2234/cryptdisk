// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// STD
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <exception>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <regex>

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/range.hpp>
#include <boost/system/system_error.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

// Windows
#include <Windows.h>
#include <WinIoCtl.h>
#include <Dbt.h>
#include <winsvc.h>
#include <Shlobj.h>

// ATL
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>
#include <atlsafe.h>
#include <comdef.h>
#include <initguid.h>
#include <wbemidl.h>

#include "CryptoLib.h"

#include "../SOURCE/Core/scope_exit.h"

// Definitions
#define USER_MODE
#define NOMINMAX
