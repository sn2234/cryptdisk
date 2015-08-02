// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>

#include <stdio.h>
#include <tchar.h>

// STL
#include <algorithm>
#include <vector>
#include <iostream>

// Boost
#define BOOST_TEST_MODULE CryptDiskTests
#include <boost/test/unit_test.hpp>
#include <boost/range.hpp>

// CryptoLib
#include <CryptoLib.h>

#include "Platform.h"

#define _TEST_MODE_
