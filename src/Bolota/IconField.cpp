/**
 * DateField.cpp
 * A Bolota field with an associated icon.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "IconField.h"

#include "Exceptions/Exceptions.h"

using namespace Bolota;

/*
 * Available icons list.
 */
static const field_icon_t IconField_IconList[BOLOTA_FIELD_ICON_NUM] = {
	// Begin field icons.
	BOLOTA_ICON_BATTERY,
	BOLOTA_ICON_BOX,
	BOLOTA_ICON_CALENDAR,
	BOLOTA_ICON_CAMERA,
	BOLOTA_ICON_CHECK,
	BOLOTA_ICON_CLIPBOARD,
	BOLOTA_ICON_CLOCK,
	BOLOTA_ICON_CPU,
	BOLOTA_ICON_FIND,
	BOLOTA_ICON_FOLDER,
	BOLOTA_ICON_GEAR,
	BOLOTA_ICON_HELP,
	BOLOTA_ICON_HISTORY,
	BOLOTA_ICON_LAPTOP,
	BOLOTA_ICON_LIGHT,
	BOLOTA_ICON_LOVE,
	BOLOTA_ICON_MEN,
	BOLOTA_ICON_MONEY,
	BOLOTA_ICON_MOVIE,
	BOLOTA_ICON_PLUS,
	BOLOTA_ICON_REDO,
	BOLOTA_ICON_REMOVE,
	BOLOTA_ICON_SIGNPOST,
	BOLOTA_ICON_SOUND,
	BOLOTA_ICON_STAR,
	BOLOTA_ICON_STOP,
	BOLOTA_ICON_TAGS,
	BOLOTA_ICON_TRASH,
	BOLOTA_ICON_UNDO,
	BOLOTA_ICON_WOMAN,
	BOLOTA_ICON_WRENCH
	// End of field icons.
};
const field_icon_t *IconField::IconList = IconField_IconList;

/*
 * +===========================================================================+
 * |                                                                           |
 * |                                 Helpers                                   |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Creates a duplicate of the field with the option of taking its place in the
 * linked list.
 *
 * @param field    Field to be copied over.
 * @param bReplace Should we change references in its relatives to point to the
 *                 new copied object?
 */
void IconField::Copy(const IconField *field, bool bReplace) {
	// Copy field base.
	Field::Copy(field, bReplace);

	// Copy specific properties.
	SetIconIndex(field->IconIndex());
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                           Getters and Setters                             |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the field icon index.
 *
 * @return Field icon index.
 */
field_icon_t IconField::IconIndex() const {
	return m_icon_index;
}

/**
 * Sets the field icon index.
 *
 * @param index New field icon index.
 */
void IconField::SetIconIndex(field_icon_t index) {
	m_icon_index = index;
}

/**
 * Sets the field icon index.
 *
 * @param index New field icon index.
 */
void IconField::SetIconIndex(uint8_t index) {
	m_icon_index = (field_icon_t)index;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                                Overrides                                  |
 * |                                                                           |
 * +===========================================================================+
 */

uint16_t IconField::FieldLength() const {
	return Field::FieldLength() + sizeof(uint8_t);
}

uint8_t IconField::ReadField(HANDLE hFile, size_t *bytes) {
	DWORD dwRead = 0;

	// Read the field's base.
	uint8_t depth = Field::ReadField(hFile, bytes);

	// Read the icon index.
	uint8_t index = 0;
	if (!::ReadFile(hFile, &index, sizeof(uint8_t), &dwRead, NULL))
		throw ReadError(hFile, *bytes, true);
	*bytes += dwRead;
	SetIconIndex(index);

	return depth;
}

size_t IconField::Write(HANDLE hFile) const {
	// Write base of the field.
	size_t ulBytes = Field::Write(hFile);
	DWORD dwWritten = 0;

	// Write the icon index.
	uint8_t index = (uint8_t)m_icon_index;
	::WriteFile(hFile, &index, sizeof(uint8_t), &dwWritten, NULL);
	ulBytes += dwWritten;

	return ulBytes;
}
