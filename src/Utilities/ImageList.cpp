/**
 * ImageList.cpp
 * Wraps and makes ImageLists more easy to deal with.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "ImageList.h"

#include <tchar.h>

#include "../Exceptions/SystemException.h"
#include "../SharedResources.h"

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
		throw SystemException("Failed to create ImageList");
	m_wResources = (WORD *)LocalAlloc(LMEM_FIXED, usNumImages * sizeof(WORD));
	for (i = 0; i < usNumImages; ++i)
		m_wResources[i] = 0;

	// Initialize labels list.
	m_usLength = 0;
	m_usNumImages = usNumImages;
	m_szLabels = (LPTSTR *)LocalAlloc(LMEM_FIXED, usNumImages * sizeof(LPTSTR));
	for (i = 0; i < usNumImages; ++i)
		m_szLabels[i] = NULL;
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
		m_usLength = 0;
	}
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
		throw std::exception("Cannot add more icons to the ImageList as it's "
			"already full");
	}

	// Add the icon to the ImageList.
	HICON hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(wResId));
	int idx = ImageList_AddIcon(m_hIml, hIcon);
	if (idx == -1)
		throw SystemException("Failed to add icon to ImageList");
	m_wResources[m_usLength] = wResId;

	// Add the label to the labels list.
	m_szLabels[m_usLength] = _tcsdup(szLabel);
	m_usLength++;

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
	throw std::exception("Cannot find the specified label in the ImageList");
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
	throw std::exception("Cannot find the specified resource ID in the "
		"ImageList");
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
