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

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/range.hpp>

// Windows
#include <Windows.h>
#include <WinIoCtl.h>
#include <Dbt.h>
#include <winsvc.h>
#include <Shlobj.h>

#include "CryptoLib.h"
// Definitions
#define USER_MODE
