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

// Generic message box.
int MsgBox(HWND hwndParent, UINT uType, LPCTSTR szTitle, LPCTSTR szText);

// Commonly used message boxes.
int MsgBoxInfo(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText);
int MsgBoxWarning(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText);
int MsgBoxError(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText);
int MsgBoxSystemError(HWND hwndParent, DWORD dwError);
int MsgBoxLastError(HWND hwndParent);
int MsgBoxBolotaError(HWND hwndParent, LPCTSTR szTitle);

#endif // _WINCOMMON_MSGBOXES_H
