// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _BOLOTA_STDAFX_H
#define _BOLOTA_STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers.
// Windows Header Files
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commdlg.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ RunTime Header Files
#include <stdexcept>
#include <iostream>
#include <string>

// Create the equivalent of TCHAR for C++ strings.
#ifdef UNICODE
	#define tstring std::wstring
#else
	#define tstring std::string
#endif // UNICODE

// Create the equivalent of TCHAR for C++ cout.
#ifdef UNICODE
	#define tcout std::wcout
#else
	#define tcout std::cout
#endif // UNICODE

// Resource definitions.
#if _MSC_VER == 1700
	#include "../vs2012/Resource.h"
#endif // _MSC_VER == 1700
#if _MSC_VER == 1200
	#include "../vc6/resource.h"
#endif // _MSC_VER == 1200
#include "SharedResources.h"

// Shims for older versions.
#if _MSC_VER <= 1200
	#include <newcpp.h>
#endif // _MSC_VER == 1200

// Utilities
#include "Utilities/MsgBoxes.h"
#include "Utilities/WindowUtilities.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // _BOLOTA_STDAFX_H
