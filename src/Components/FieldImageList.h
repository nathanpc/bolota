/**
 * FieldImageList.h
 * Field icons ImageList.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _COMPONENTS_FIELDIMAGELIST_H
#define _COMPONENTS_FIELDIMAGELIST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Utilities/ImageList.h"

/**
 * Field icons ImageList.
 */
class FieldImageList : public ImageList {
protected:
	UINT8 m_usIndexCalendar;
	UINT8 m_usIndexOffset;

public:
	FieldImageList(HINSTANCE hInst);

	// Common icons.
	UINT8 Calendar() const;

	// Getters
	UINT8 IndexFromFieldIndex(UINT8 usIndex) const;
};

#endif // _COMPONENTS_FIELDIMAGELIST_H
