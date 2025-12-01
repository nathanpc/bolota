/**
 * ImageList.cpp
 * Wraps and makes ImageLists more easy to deal with.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "ImageList.h"

#include <tchar.h>

#include "../../Bolota/Errors/SystemError.h"
#include "../SharedResources.h"

// Fixes a problem in Visual Studio 2005.
#ifndef ImageList_AddIcon
	#define ImageList_AddIcon(himl, hicon) ImageList_ReplaceIcon(himl, -1, hicon)
#endif // !ImageList_AddIcon

using namespace Bolota;

/**
 * Creates an ImageList with a specific size for the icons.
 *
 * @param hInst      Application's instance handle.
 * @param usSize     Icon size in pixels.
 * @param usBitDepth Bit depth of the images in the ImageList.
 * 
 */
ImageList::ImageList(HINSTANCE hInst, UINT8 usSize, UINT8 usBitDepth,
					 UINT8 usNumImages) {
	UINT8 i = 0;

	// Initialize ImageList.
	m_hInst = hInst;
	m_usSize = usSize;
	m_hIml = ImageList_Create(usSize, usSize, usBitDepth, usNumImages, 0);
	if (m_hIml == NULL)
		ThrowError(new SystemError(EMSG("Failed to create ImageList")));
	m_wResources = (WORD *)LocalAlloc(LMEM_FIXED, usNumImages * sizeof(WORD));
	for (i = 0; i < usNumImages; ++i)
		m_wResources[i] = 0;

	// Initialize labels list.
	m_usLength = 0;
	m_usNumImages = usNumImages;
	m_szLabels = (LPTSTR *)LocalAlloc(LMEM_FIXED, usNumImages * sizeof(LPTSTR));
	for (i = 0; i < usNumImages; ++i)
		m_szLabels[i] = NULL;

#ifdef UNDER_CE
	// Initialize HICON list.
	m_ahIcons = (HICON *)LocalAlloc(LMEM_FIXED, usNumImages * sizeof(HICON));
#endif // UNDER_CE
}

/**
 * Destroys the ImageList and any resources allocated for it.
 */
ImageList::~ImageList() {
	// Destroy the ImageList handle.
	if (m_hIml)
		ImageList_Destroy(m_hIml);
	m_hIml = NULL;

	// Free resources ID list.
	if (m_wResources)
		LocalFree(m_wResources);
	m_wResources = NULL;

	// Clean up the labels list.
	if (m_szLabels) {
		for (UINT8 i = 0; i < m_usLength; ++i) {
			if (m_szLabels[i] != NULL)
				free(m_szLabels[i]);
		}
		LocalFree(m_szLabels);
		m_szLabels = NULL;
	}

#ifdef UNDER_CE
	// Clean up the HICON list.
	if (m_ahIcons) {
		for (UINT8 i = 0; i < m_usLength; ++i) {
			if (m_ahIcons[i] != NULL)
				DestroyIcon(m_ahIcons[i]);
		}
		LocalFree(m_ahIcons);
		m_ahIcons = NULL;
	}
#endif // UNDER_CE

	m_usLength = 0;
}

/**
 * Adds an icon to the ImageList by its resource ID.
 *
 * @param szLabel Label to be associated with this icon.
 * @param wResId  Icon's resource ID.
 *
 * @return Added icon's index in the ImageList.
 */
UINT8 ImageList::AddIcon(LPCTSTR szLabel, WORD wResId) {
	// Check if we have space to add another icon to the list.
	if (m_usLength == m_usNumImages) {
		ThrowError(EMSG("Cannot add more icons to the ImageList as it's ")
			_T("already full"));
		return BOLOTA_ERR_UINT8;
	}

	// Add the icon to the ImageList.
#ifndef UNDER_CE
	HICON hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(wResId));
#else
	HICON hIcon = (HICON)LoadImage(m_hInst, MAKEINTRESOURCE(wResId), IMAGE_ICON,
		m_usSize, m_usSize, 0);
	m_ahIcons[m_usLength] = hIcon;
#endif // !UNDER_CE
	int idx = ImageList_AddIcon(m_hIml, hIcon);
	if (idx == -1) {
		ThrowError(new SystemError(EMSG("Failed to add icon to ImageList")));
		return BOLOTA_ERR_UINT8;
	}
	m_wResources[m_usLength] = wResId;

	// Add the label to the labels list.
	m_szLabels[m_usLength] = _tcsdup(szLabel);
	m_usLength++;

	return static_cast<UINT8>(idx);
}

/**
 * Adds a bitmap resource with multiple images to the ImageList.
 * 
 * @warning ImageList must be initialized with ILC_MASK flag in order for
 *          transparency to work.
 *
 * @param wResId Icon's resource ID.
 * @param usNum  Number of icons in the bitmap.
 * @param crMask Color mask of the background for transparency.
 * 
 * @return Index of the first image in the bitmap on the ImageList.
 */
UINT8 ImageList::AddBitmap(WORD wResId, UINT8 usNum, COLORREF crMask) {
	// Check if we have space to add more icons icon to the list.
	if ((m_usLength + usNum) > m_usNumImages) {
		ThrowError(EMSG("ImageList does not have space for bitmap"));
		return BOLOTA_ERR_UINT8;
	}

	// Load the bitmap resource.
	HBITMAP hbm = (HBITMAP)LoadImage(m_hInst, MAKEINTRESOURCE(wResId),
		IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	if (hbm == NULL) {
		ThrowError(new SystemError(EMSG("Failed to load bitmap")));
		return BOLOTA_ERR_UINT8;
	}

	// Add the bitmap.
	int idx = ImageList_AddMasked(m_hIml, hbm, crMask);
	if (idx == -1) {
		ThrowError(new SystemError(EMSG("Failed to add bitmap to ImageList")));
		return BOLOTA_ERR_UINT8;
	}

	// Destroy the temporary bitmap handle.
	DeleteObject(hbm);
	hbm = NULL;

	// Pad the internal structures.
	for (UINT8 i = 0; i < usNum; ++i) {
		m_wResources[m_usLength] = 0;
		m_szLabels[m_usLength] = NULL;
		m_usLength++;
	}

	return static_cast<UINT8>(idx);
}

/**
 * Adds a blank icon to the ImageList.
 *
 * @return Index of the blank icon in the ImageList.
 */
UINT8 ImageList::AddBlank() {
	return AddIcon(_T(""), IDI_BLANK);
}

/**
 * Gets the ImageList handle associated with this object.
 *
 * @return ImageList handle.
 */
HIMAGELIST ImageList::Handle() const {
	return m_hIml;
}

/**
 * Gets the index of an specific icon in the ImageList given its label.
 *
 * @warning Throws an exception if the specified label wasn't found.
 *
 * @param szLabel Label associated with the icon in the ImageList.
 *
 * @return Index of the associated icon.
 */
UINT8 ImageList::GetIndex(LPCTSTR szLabel) const {
	// Search for the specified label.
	for (UINT8 i = 0; i < m_usLength; ++i) {
		if (!_tcscmp(m_szLabels[i], szLabel))
			return i;
	}

	// Looks like it's not one of our labels.
	ThrowError(EMSG("Cannot find the specified label in the ImageList"));
	return BOLOTA_ERR_UINT8;
}

/**
 * Gets the index of an specific icon in the ImageList given its resource ID.
 *
 * @warning Throws an exception if the specified resource ID wasn't found.
 *
 * @param wResId Resource ID associated with the icon in the ImageList.
 *
 * @return Index of the associated icon.
 */
UINT8 ImageList::GetIndex(WORD wResId) const {
	// Search for the specified label.
	for (UINT8 i = 0; i < m_usLength; ++i) {
		if (m_wResources[i] == wResId)
			return i;
	}

	// Looks like it's not one of our labels.
	ThrowError(EMSG("Cannot find the specified resource ID in the ImageList"));
	return BOLOTA_ERR_UINT8;
}

/**
 * Gets the label associated with an icon at an index.
 *
 * @warning No bounds checking or validity check is done by this method.
 *
 * @param usIndex Index of the icon in the ImageList.
 *
 * @return Label associated with the icon.
 */
LPCTSTR ImageList::GetLabel(UINT8 usIndex) const {
	return m_szLabels[usIndex];
}

/**
 * Gets the resource ID associated with an icon at an index.
 *
 * @warning No bounds checking or validity check is done by this method.
 *
 * @param usIndex Index of the icon in the ImageList.
 *
 * @return Resource ID associated with the icon.
 */
WORD ImageList::GetResourceID(UINT8 usIndex) const {
	return m_wResources[usIndex];
}

/**
 * Gets the size (CX and CY) of the image in this ImageList.
 * 
 * @return Size of the images in pixels.
 */
UINT8 ImageList::IconSize() const {
	return m_usSize;
}
