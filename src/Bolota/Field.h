/**
 * Field.h
 * Field object abstraction of a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_FIELD_H
#define _BOLOTA_FIELD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdint.h>

#ifdef __cplusplus
#include "UString.h"

extern "C" {
#endif // __cplusplus

/**
 * An 8-bit value that determines the type of a field.
 */
typedef enum bolota_type_e {
	BOLOTA_TYPE_IGNORE = 0,
	BOLOTA_TYPE_TEXT = 'T',
	BOLOTA_TYPE_DATE = 'd'
} bolota_type_t;

/**
 * A line of a note in a document.
 */
typedef struct bolota_field_s {
	/* Header */
	bolota_type_t type;  /* Field type. (uint8) */
	uint8_t depth;       /* Level of indentation of the topic in the note. */
	uint16_t length;     /* Length of the data part of the field (does not
                          * include header). */

	/* Data */
	union {
		char *text;      /* Text associated with the field. (not NUL terminated */
		uint8_t *data;   /* when saved to file) */
	};
} bolota_field_t;

#ifdef __cplusplus
}

namespace Bolota {
	/**
	 * Field object abstraction of a Bolota document.
	 */
	class Field {
	private:
		// Internals
		bolota_type_t m_type;
		UString *m_text;

		// Linked list.
		Field *m_parent;
		Field *m_child;
		Field *m_prev;
		Field *m_next;

	public:
		// Constructors and destructors.
		Field(bolota_type_t type);
		Field(Field *parent, bolota_type_t type);
		virtual ~Field();
		void Destroy(bool include_child, bool include_next);

		// Getters and setters.
		bolota_type_t Type() const;
		void SetType(bolota_type_t type);
		uint8_t Depth() const;
		bool HasText() const;
		UString* Text() const;
		void SetText(const char *mbstr);
		void SetText(const wchar_t *wstr);
		void SetTextOwner(char *mbstr);
		void SetTextOwner(wchar_t *wstr);
		bolota_field_t Struct() const;

		// Linked list.
		bool HasParent() const;
		Field* Parent() const;
		void SetParent(Field *parent);
		bool HasChild() const;
		Field* Child() const;
		void SetChild(Field *child);
		bool HasPrevious() const;
		Field* Previous() const;
		void SetPrevious(Field *prev);
		bool HasNext() const;
		Field* Next() const;
		void SetNext(Field *next);

	protected:
		// Constructor helper.
		void Initialize(bolota_type_t type, UString *text, Field *parent,
			Field *child, Field *prev, Field *next);
	};

	/**
	 * A Bolota text-type field.
	 */
	class TextField : public Field {
	public:
		TextField() : Field(BOLOTA_TYPE_TEXT) {};
		TextField(Field *parent) : Field(parent, BOLOTA_TYPE_TEXT) {};

		TextField(Field *parent, const char *mbstr) : Field(parent, BOLOTA_TYPE_TEXT) {
			SetText(mbstr);
		};
		TextField(const char *mbstr) : Field(BOLOTA_TYPE_TEXT) {
			SetText(mbstr);
		};

		TextField(Field *parent, const wchar_t *wstr) : Field(parent, BOLOTA_TYPE_TEXT) {
			SetText(wstr);
		};
		TextField(const wchar_t *wstr) : Field(BOLOTA_TYPE_TEXT) {
			SetText(wstr);
		};
	};

	/**
	 * A Bolota date-type field.
	 */
	class DateField : public Field {
	public:
		DateField() : Field(BOLOTA_TYPE_DATE) {};
	};
}

#endif // __cplusplus

#endif // _BOLOTA_FIELD_H
