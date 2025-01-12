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
#include <commctrl.h>
#include <commdlg.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ RunTime Header Files
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

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	// Pocket PC shell stuff.
	#define SHELL_AYGSHELL
	#include <aygshell.h>
	#include <sipapi.h>
#endif // defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)

// Resource definitions.
#if _MSC_VER >= 1900
	#include "../vs2022/Resource.h"
#elif _MSC_VER == 1700
	#include "../vs2012/Resource.h"
#elif _MSC_VER == 1200
	#include "../vc6/resource.h"
#endif // _MSC_VER == 1200
#include "SharedResources.h"

// Shims for older versions.
#if _MSC_VER <= 1200
	#include <newcpp.h>
#endif // _MSC_VER == 1200

// Shims for Windows CE.
#ifdef UNDER_CE
	#include <stdint.h>
	#include <wce_defs.h>
#endif // UNDER_CE

// Error handling.
#include "Bolota/Errors/Error.h"

// Utilities
#include "Utilities/MsgBoxes.h"
#include "Utilities/WindowUtilities.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // _BOLOTA_STDAFX_H
