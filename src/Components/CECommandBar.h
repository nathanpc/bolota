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

	public:
		// Constructors and destructors.
		CommandBar(HINSTANCE hInst, HWND hwndParent);
		virtual ~CommandBar();

		// Helpers
		LONG Height() const;
		HWND Handle() const;
	};

}

#endif // _COMPONENTS_CECOMMANDBAR_H
