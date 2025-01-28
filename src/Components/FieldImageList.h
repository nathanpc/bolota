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
#include "../Bolota/IconField.h"

/**
 * Field icons ImageList.
 */
class FieldImageList : public ImageList {
protected:
	UINT8 m_usIndexBlank;
	UINT8 m_usIndexBullet;
	UINT8 m_usIndexCalendar;
	UINT8 m_usIndexOffset;

public:
	FieldImageList(HINSTANCE hInst);

	// Common icons.
	UINT8 Blank() const;
	UINT8 Bullet() const;
	UINT8 Calendar() const;

	// Getters
	static UINT8 NumAvailableIcons() { return BOLOTA_FIELD_ICON_NUM; };
	LPCTSTR LabelFromFieldIndex(field_icon_t usIndex) const;
	UINT8 IndexFromFieldIndex(field_icon_t usIndex) const;
};

#endif // _COMPONENTS_FIELDIMAGELIST_H
