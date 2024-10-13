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
#include "Components/BolotaView.h"
#include "FieldManagerDialog.h"

class MainWindow {
private:
	// UI components.
	BolotaView *m_wndBolota;

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

	// Event handlers.
	LRESULT OnMenuCommand(UINT_PTR wmId, UINT_PTR wmEvent);
	LRESULT OpenFieldManager(FieldManagerDialog::DialogType type);
};

#endif // _BOLOTA_MAINWINDOW_H
