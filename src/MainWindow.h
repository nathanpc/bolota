/**
 * MainWindow.h
 * Main window of the application.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_MAINWINDOW_H
#define _BOLOTA_MAINWINDOW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class MainWindow {
private:

public:
	// Global handles.
	HINSTANCE hInst;
	HWND hWnd;

	// Constructors and destructors.
	MainWindow(HINSTANCE hInstance, LPCTSTR szURI);
	virtual ~MainWindow();

	// Controls setup.
	BOOL SetupControls(HWND hWnd);
	BOOL ResizeWindows(HWND hwndParent);
};

#endif // _BOLOTA_MAINWINDOW_H
