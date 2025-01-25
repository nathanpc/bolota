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
#ifndef UNDER_CE
	#include "Components/Toolbar.h"
#else
	#include "Components/CECommandBar.h"
#endif // !UNDER_CE

class MainWindow {
private:
	// UI components.
	BolotaView *m_wndBolota;
#ifndef UNDER_CE
	Toolbar *m_toolBar;
#else
	WinCE::CommandBar *m_cmdBar;
#endif // !UNDER_CE

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
	bool OnContextMenu(HWND hWnd, int xPos, int yPos);
	bool OnClose() const;

#ifdef UNDER_CE
	WinCE::CommandBar* CommandBar() const;
#endif // UNDER_CE
};

#endif // _BOLOTA_MAINWINDOW_H
