/**
 * ImageList.h
 * Wraps and makes ImageLists more easy to deal with.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UTILITIES_IMAGELIST_H
#define _UTILITIES_IMAGELIST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <commctrl.h>
#include <stdint.h>

/**
 * Wraps and makes ImageLists more easy to deal with.
 */
class ImageList {
protected:
	HINSTANCE m_hInst;
	HIMAGELIST m_hIml;
	UINT8 m_usSize;
	UINT8 m_usNumImages;
	
	WORD *m_wResources;
	LPTSTR *m_szLabels;
	UINT8 m_usLength;

public:
	// Constructors and destructors.
	ImageList(HINSTANCE hInst, UINT8 usSize, UINT8 usBitDepth,
		UINT8 usNumImages);
	virtual ~ImageList();

	// List manipulation.
	UINT8 AddIcon(LPCTSTR szLabel, WORD wResId);
	UINT8 AddBlank();

	// Getters
	HIMAGELIST Handle() const;
	UINT8 GetIndex(LPCTSTR szLabel) const;
	UINT8 GetIndex(WORD wResId) const;
	LPCTSTR GetLabel(UINT8 usIndex) const;
	WORD GetResourceID(UINT8 usIndex) const;
};

#endif // _UTILITIES_IMAGELIST_H
