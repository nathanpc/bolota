/**
 * DateField.h
 * A Bolota field representing a timestamp.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_DATEFIELD_H
#define _BOLOTA_DATEFIELD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Field.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * A bolota timestamp structure. Used for determining time in a system-agnostic
 * and retro-friendly (Y2K38-compliant even on older systems) way.
 */
typedef struct timestamp_s {
	uint16_t year;
	uint8_t month;  /* Starts at 1. */
	uint8_t day;    /* Starts at 1. */
	uint8_t hour;   /* Always in UTC. */
	uint8_t minute;
	uint8_t second;

	uint8_t reserved;  /* Struct padding. Always 0. */
} timestamp_t;

/**
 * A structure representing a date field in the document.
 */
typedef struct bolota_date_field_s {
	bolota_field_t base;
	timestamp_t timestamp;
} bolota_date_field_t;

#ifdef __cplusplus
}

namespace Bolota {
	/**
	 * A Bolota field representing a timestamp.
	 */
	class DateField : public Field {
	protected:
		timestamp_t m_ts;

	public:
		// Constructors
		DateField() : Field(BOLOTA_TYPE_DATE) { InitializeDate(NULL); };
		DateField(const char *mbstr) : Field(BOLOTA_TYPE_DATE, mbstr) {
			InitializeDate(NULL);
		};
		DateField(const wchar_t *wstr) : Field(BOLOTA_TYPE_DATE, wstr) {
			InitializeDate(NULL);
		};
		DateField(const timestamp_t *ts) : Field(BOLOTA_TYPE_DATE) {
			InitializeDate(ts);
		};
		DateField(const timestamp_t *ts, const char *mbstr) :
			Field(BOLOTA_TYPE_DATE, mbstr) {
			InitializeDate(ts);
		};
		DateField(const timestamp_t *ts, const wchar_t *wstr) :
			Field(BOLOTA_TYPE_DATE, wstr) {
			InitializeDate(ts);
		};
		DateField(Field *parent) :
			Field(parent, BOLOTA_TYPE_DATE) {
			InitializeDate(NULL);
		};
		DateField(Field *parent, const char *mbstr) :
			Field(parent, BOLOTA_TYPE_DATE, mbstr) {
			InitializeDate(NULL);
		};
		DateField(Field *parent, const wchar_t *wstr) :
			Field(parent, BOLOTA_TYPE_DATE, wstr) {
			InitializeDate(NULL);
		};
		DateField(Field *parent, const timestamp_t *ts) :
			Field(parent, BOLOTA_TYPE_DATE) {
			InitializeDate(ts);
		};
		DateField(Field *parent, const timestamp_t *ts, const char *mbstr) :
			Field(parent, BOLOTA_TYPE_DATE, mbstr) {
			InitializeDate(ts);
		};
		DateField(Field *parent, const timestamp_t *ts, const wchar_t *wstr) :
			Field(parent, BOLOTA_TYPE_DATE, wstr) {
			InitializeDate(ts);
		};
		
		// Helpers
		virtual void Copy(const DateField *field, bool bReplace);
		static DateField* Now();
		void RefreshText();
		SYSTEMTIME ToSystemTime() const;

		// Overrides
		uint16_t FieldLength() const override;
		uint8_t ReadField(HANDLE hFile, size_t *bytes) override;
		size_t Write(HANDLE hFile) const override;

		// Getters and setters.
		timestamp_t Timestamp() const;
		void SetTimestamp(const timestamp_t *ts);
		void SetTimestamp(const SYSTEMTIME *st);

	protected:
		// Helpers
		void InitializeDate(const timestamp_t *ts);
	};
}

#endif // __cplusplus

#endif // _BOLOTA_DATEFIELD_H
