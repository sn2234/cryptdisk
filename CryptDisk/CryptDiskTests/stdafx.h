// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

// STL
#include <algorithm>
#include <vector>

// Boost
#define BOOST_TEST_MODULE CryptDiskTests
#include <boost/test/unit_test.hpp>
#include <boost/range.hpp>

// CryptoLib
#include <CryptoLib.h>

#include "Platform.h"
