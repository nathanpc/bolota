/**
 * WindowUtilities.h
 * Some utility functions to help us deal with Windows's windows and controls.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _WINCOMMON_WINDOWUTILITIES_H
#define _WINCOMMON_WINDOWUTILITIES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdarg.h>

LPTSTR GetWindowTextAlloc(HWND hWnd);
BOOL SetWindowFormatText(HWND hWnd, LPCTSTR szFormat, ...);

void SetDlgDefaultButton(HWND hDlg, int nID);

void RectScreenToClient(RECT *rc, HWND hWnd);

#endif // _WINCOMMON_WINDOWUTILITIES_H
