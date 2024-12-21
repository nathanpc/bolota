/**
 * MsgBoxes.h
 * Some utility functions to work with Message Boxes more easily and write less
 * boilerplate for something that is so simple.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _WINCOMMON_MSGBOXES_H
#define _WINCOMMON_MSGBOXES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#ifndef UNDER_CE
	#include <stdexcept>
#endif // !UNDER_CE

#include "../Bolota/Exceptions/Error.h"

// Generic message box.
int MsgBox(HWND hwndParent, UINT uType, LPCTSTR szTitle, LPCTSTR szText);

// Commonly used message boxes.
int MsgBoxInfo(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText);
int MsgBoxWarning(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText);
int MsgBoxError(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText);
int MsgBoxLastError(HWND hwndParent);
#ifndef UNDER_CE
int MsgBoxException(HWND hwndParent, const std::exception& exc,
					LPCTSTR szTitle);
#endif // !UNDER_CE
int MsgBoxBolotaError(HWND hwndParent, Bolota::Error& error, LPCTSTR szTitle);

#endif // _WINCOMMON_MSGBOXES_H
