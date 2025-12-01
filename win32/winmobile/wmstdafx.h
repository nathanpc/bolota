// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _BOLOTA_WMSTDAFX_H
#define _BOLOTA_WMSTDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

// NOTE - this value is not strongly correlated to the Windows CE OS version being targeted
//#define WINVER _WIN32_WCE

#include <ceconfig.h>

#ifdef _CE_DCOM
	#define _ATL_APARTMENT_THREADED
#endif // _CE_DCOM

#ifdef SHELL_AYGSHELL
	#include <aygshell.h>
	#pragma comment(lib, "aygshell.lib") 
#endif // SHELL_AYGSHELL

#if (_MSC_VER >= 1400) && (_WIN32_WCE > 300)
	#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
		#ifndef _DEVICE_RESOLUTION_AWARE
			#define _DEVICE_RESOLUTION_AWARE
		#endif // !_DEVICE_RESOLUTION_AWARE
	#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

	#ifdef _DEVICE_RESOLUTION_AWARE
		#include "DeviceResolutionAware.h"
	#endif // _DEVICE_RESOLUTION_AWARE

	#if _WIN32_WCE < 0x500 && (defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP))
		#pragma comment(lib, "ccrtrtti.lib")
		#ifdef _X86_	
			#if defined(_DEBUG)
				#pragma comment(lib, "libcmtx86d.lib")
			#else
				#pragma comment(lib, "libcmtx86.lib")
			#endif
		#endif
	#endif

	#include <altcecrt.h>
#endif // _MSC_VER >= 1400

// TODO: reference additional headers your program requires here

#endif // _BOLOTA_WMSTDAFX_H