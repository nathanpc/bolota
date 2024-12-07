/**
 * FieldImageList.cpp
 * Field icons ImageList.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "FieldImageList.h"

#include "../stdafx.h"

// Constant definitions.
#define FIL_NUM_ICONS (BOLOTA_FIELD_ICON_NUM + 2)
#define FIL_BIT_DEPTH ILC_COLORDDB

/**
 * Creates and populates the Field icon ImageList.
 *
 * @param hInst Application's instance handle.
 */
FieldImageList::FieldImageList(HINSTANCE hInst) :
	ImageList(hInst, GetSystemMetrics(SM_CXSMICON), ILC_MASK | FIL_BIT_DEPTH,
	FIL_NUM_ICONS) {
	m_usIndexOffset = 0;

	// Add internal icons.
	AddBlank();
	m_usIndexOffset++;
	m_usIndexBullet = AddIcon(_T("Bullet"), IDI_FI_BULLET1);
	m_usIndexOffset++;

	// Begin field icons.
	AddIcon(_T("Battery"), IDI_FI_BATTERY);
	AddIcon(_T("Box"), IDI_FI_BOX);
	m_usIndexCalendar = AddIcon(_T("Calendar"), IDI_FI_CALENDAR);
	AddIcon(_T("Camera"), IDI_FI_CAMERA);
	AddIcon(_T("Check"), IDI_FI_CHECK);
	AddIcon(_T("Clipboard"), IDI_FI_CLIPBOARD);
	AddIcon(_T("Clock"), IDI_FI_CLOCK);
	AddIcon(_T("CPU"), IDI_FI_CPU);
	AddIcon(_T("Find"), IDI_FI_FIND);
	AddIcon(_T("Folder"), IDI_FI_FOLDER);
	AddIcon(_T("Gear"), IDI_FI_GEAR);
	AddIcon(_T("Help"), IDI_FI_HELP);
	AddIcon(_T("History"), IDI_FI_HISTORY);
	AddIcon(_T("Laptop"), IDI_FI_LAPTOP);
	AddIcon(_T("Light"), IDI_FI_LIGHT);
	AddIcon(_T("Love"), IDI_FI_LOVE);
	AddIcon(_T("Men"), IDI_FI_MEN);
	AddIcon(_T("Money"), IDI_FI_MONEY);
	AddIcon(_T("Movie"), IDI_FI_MOVIE);
	AddIcon(_T("Plus"), IDI_FI_PLUS);
	AddIcon(_T("Redo"), IDI_FI_REDO);
	AddIcon(_T("Remove"), IDI_FI_REMOVE);
	AddIcon(_T("Signpost"), IDI_FI_SIGNPOST);
	AddIcon(_T("Sound"), IDI_FI_SOUND);
	AddIcon(_T("Star"), IDI_FI_STAR);
	AddIcon(_T("Stop"), IDI_FI_STOP);
	AddIcon(_T("Tags"), IDI_FI_TAGS);
	AddIcon(_T("Trash"), IDI_FI_TRASH);
	AddIcon(_T("Undo"), IDI_FI_UNDO);
	AddIcon(_T("Woman"), IDI_FI_WOMAN);
	AddIcon(_T("Wrench"), IDI_FI_WRENCH);
	// End of field icons.
}

/**
 * Gets the default bullet field icon index.
 *
 * @return Default bullet icon index.
 */
UINT8 FieldImageList::Bullet() const {
	return m_usIndexBullet;
}

/**
 * Gets the calendar field icon index.
 *
 * @return Calendar icon index.
 */
UINT8 FieldImageList::Calendar() const {
	return m_usIndexCalendar;
}

/**
 * Gets the icon label in the ImageList from a Bolota field icon index.
 *
 * @param usIndex Bolota field icon index.
 *
 * @return ImageList icon label.
 */
LPCTSTR FieldImageList::LabelFromFieldIndex(field_icon_t usIndex) const {
	return GetLabel(IndexFromFieldIndex(usIndex));
}

/**
 * Gets the icon index in the ImageList from a Bolota field icon index.
 *
 * @param usIndex Bolota field icon index.
 *
 * @return ImageList icon index.
 */
UINT8 FieldImageList::IndexFromFieldIndex(field_icon_t usIndex) const {
	return usIndex + m_usIndexOffset - 1;
}
