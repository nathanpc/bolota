/**
 * IconField.h
 * A Bolota field with an associated icon.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_ICONFIELD_H
#define _BOLOTA_ICONFIELD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Field.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Number of field icons defined.
 */
#define BOLOTA_FIELD_ICON_NUM 31

/**
 * The list of field icons and their associated IDs.
 */
typedef enum field_icon_e {
	// Begin field icons.
	BOLOTA_ICON_NONE      = 0,
	BOLOTA_ICON_BATTERY   = 1,
	BOLOTA_ICON_BOX       = 2,
	BOLOTA_ICON_CALENDAR  = 3,
	BOLOTA_ICON_CAMERA    = 4,
	BOLOTA_ICON_CHECK     = 5,
	BOLOTA_ICON_CLIPBOARD = 6,
	BOLOTA_ICON_CLOCK     = 7,
	BOLOTA_ICON_CPU       = 8,
	BOLOTA_ICON_FIND      = 9,
	BOLOTA_ICON_FOLDER    = 10,
	BOLOTA_ICON_GEAR      = 11,
	BOLOTA_ICON_HELP      = 12,
	BOLOTA_ICON_HISTORY   = 13,
	BOLOTA_ICON_LAPTOP    = 14,
	BOLOTA_ICON_LIGHT     = 15,
	BOLOTA_ICON_LOVE      = 16,
	BOLOTA_ICON_MEN       = 17,
	BOLOTA_ICON_MONEY     = 18,
	BOLOTA_ICON_MOVIE     = 19,
	BOLOTA_ICON_PLUS      = 20,
	BOLOTA_ICON_REDO      = 21,
	BOLOTA_ICON_REMOVE    = 22,
	BOLOTA_ICON_SIGNPOST  = 23,
	BOLOTA_ICON_SOUND     = 24,
	BOLOTA_ICON_STAR      = 25,
	BOLOTA_ICON_STOP      = 26,
	BOLOTA_ICON_TAGS      = 27,
	BOLOTA_ICON_TRASH     = 28,
	BOLOTA_ICON_UNDO      = 29,
	BOLOTA_ICON_WOMAN     = 30,
	BOLOTA_ICON_WRENCH    = 31,
	// End of field icons.
} field_icon_t;

/**
 * A structure representing a date field in the document.
 */
typedef struct bolota_icon_field_s {
	bolota_field_t base;
	field_icon_t icon_index;
} bolota_icon_field_t;

#ifdef __cplusplus
}

namespace Bolota {
	/**
	 * A Bolota field with an associated icon.
	 */
	class IconField : public Field {
	protected:
		field_icon_t m_icon_index;

	public:
		// Available icons list.
		static const field_icon_t *IconList;

		// Constructors
		IconField() : Field(BOLOTA_TYPE_ICON) {
			m_icon_index = BOLOTA_ICON_NONE;
		};
		IconField(field_icon_t index) : Field(BOLOTA_TYPE_ICON) {
			m_icon_index = index;
		};
		IconField(field_icon_t index, const char *mbstr) :
			Field(BOLOTA_TYPE_ICON, mbstr) {
			m_icon_index = index;
		};
		IconField(field_icon_t index, const wchar_t *wstr) :
			Field(BOLOTA_TYPE_ICON, wstr) {
			m_icon_index = index;
		};
		IconField(Field *parent) :
			Field(parent, BOLOTA_TYPE_ICON) {
			m_icon_index = BOLOTA_ICON_NONE;
		};
		IconField(Field *parent, const char *mbstr) :
			Field(parent, BOLOTA_TYPE_ICON, mbstr) {
			m_icon_index = BOLOTA_ICON_NONE;
		};
		IconField(Field *parent, const wchar_t *wstr) :
			Field(parent, BOLOTA_TYPE_ICON, wstr) {
			m_icon_index = BOLOTA_ICON_NONE;
		};
		IconField(Field *parent, field_icon_t index) :
			Field(parent, BOLOTA_TYPE_ICON) {
			m_icon_index = BOLOTA_ICON_NONE;
		};
		IconField(Field *parent, field_icon_t index, const char *mbstr) :
			Field(parent, BOLOTA_TYPE_ICON, mbstr) {
			m_icon_index = BOLOTA_ICON_NONE;
		};
		IconField(Field *parent, field_icon_t index, const wchar_t *wstr) :
			Field(parent, BOLOTA_TYPE_ICON, wstr) {
			m_icon_index = BOLOTA_ICON_NONE;
		};

		// Helpers
		virtual void Copy(const IconField *field, bool bReplace);

		// Overrides
		uint16_t FieldLength() const override;
		uint8_t ReadField(FHND hFile, size_t *bytes) override;
		size_t Write(FHND hFile) const override;

		// Getters and setters.
		field_icon_t IconIndex() const;
		void SetIconIndex(field_icon_t index);
		void SetIconIndex(uint8_t index);
	};
}

#endif // __cplusplus

#endif // _BOLOTA_ICONFIELD_H
