/**
 * CECommandBar.h
 * Handles the CommandBar used in some Windows CE devices.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _COMPONENTS_CECOMMANDBAR_H
#define _COMPONENTS_CECOMMANDBAR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../stdafx.h"

namespace WinCE {

	/**
	 * Handles the CommandBar used in some Windows CE devices.
	 */
	class CommandBar {
	protected:
		HINSTANCE hInst;
		HWND hwndParent;
		HWND hWnd;

#ifdef SHELL_AYGSHELL
		SHACTIVATEINFO m_sai;
#endif // SHELL_AYGSHELL

	public:
		// Constructors and destructors.
		CommandBar(HINSTANCE hInst, HWND hwndParent);
		virtual ~CommandBar();

		// Helpers
		LONG Height() const;
		HWND Handle() const;
#ifdef SHELL_AYGSHELL
		RECT SIPVisibleDesktop() const;
		SHACTIVATEINFO SHActivateInfo() const;
#endif // SHELL_AYGSHELL
	};

}

#endif // _COMPONENTS_CECOMMANDBAR_H
