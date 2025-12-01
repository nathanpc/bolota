/**
 * DateField.cpp
 * A Bolota field representing a timestamp.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "DateField.h"

#include "Errors/ErrorCollection.h"

using namespace Bolota;

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
void DateField::Copy(const DateField *field, bool bReplace) {
	// Copy field base.
	Field::Copy(field, bReplace);

	// Copy specific properties.
	timestamp_t ts = field->Timestamp();
	SetTimestamp(&ts);
}

/**
 * Initializes the date field object.
 *
 * @param ts Optional. Timestamp for the field. If set to NULL the system's
 *           current time will be fetched and used.
 */
void DateField::InitializeDate(const timestamp_t *ts) {
	// Initialize the date to a known-good value automatically.
	if (ts == NULL) {
		memset(&m_ts, 0, sizeof(timestamp_t));
		return;
	}

	// Override values.
	SetTimestamp(ts);
	m_ts.reserved = 0;
}

/**
 * Creates a brand new date field object with the current system time.
 *
 * @warning This method allocates memory dynamically.
 *
 * @return Pointer to a date field object containing the current system time.
 */
DateField* DateField::Now() {
	DateField *self = new DateField();

	// Set the timestamp to the current time.
#ifdef _WIN32
	SYSTEMTIME st;
	GetSystemTime(&st);
	self->SetTimestamp(&st);
#else
	self->SetTimestamp(time(NULL));
#endif // _WIN32
	self->RefreshText();

	return self;
}

/**
 * Updates the field's text component to match the timestamp.
 */
void DateField::RefreshText() {
	// Build up an ISO8601 timestamp.
	LPTSTR szTimestamp;
	szTimestamp = (LPTSTR)malloc(21 * sizeof(TCHAR));
	_stprintf(szTimestamp, _T("%04u-%02u-%02uT%02u:%02u:%02uZ"),
		m_ts.year, m_ts.month, m_ts.day, m_ts.hour, m_ts.minute, m_ts.second);

	// Assign the new timestamp to the field's text.
	SetTextOwner(szTimestamp);
}

#ifdef _WIN32
/**
 * Gets the field's timestamp in the system's native Date-Time format.
 *
 * @return Timestamp in the system's native Date-Time format.
 */
SYSTEMTIME DateField::ToSystemTime() const {
	SYSTEMTIME st;
	st.wYear = m_ts.year;
	st.wMonth = m_ts.month;
	st.wDay = m_ts.day;
	st.wHour = m_ts.hour;
	st.wMinute = m_ts.minute;
	st.wSecond = m_ts.second;
	st.wMilliseconds = 0;

	return st;
}
#endif // _WIN32

/*
 * +===========================================================================+
 * |                                                                           |
 * |                           Getters and Setters                             |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the timestamp of the field.
 *
 * @return Field's timestamp structure.
 */
timestamp_t DateField::Timestamp() const {
	return this->m_ts;
}

/**
 * Sets the timestamp of the field.
 *
 * @param ts New timestamp of the field.
 */
void DateField::SetTimestamp(const timestamp_t *ts) {
	this->m_ts = *ts;
}

#ifdef _WIN32
/**
 * Sets the timestamp of the field from an UTC Windows SYSTEMTIME structure.
 *
 * @param st Windows SYSTEMTIME structure.
 */
void DateField::SetTimestamp(const SYSTEMTIME *st) {
	m_ts.year = (uint16_t)st->wYear;
	m_ts.month = (uint8_t)st->wMonth;
	m_ts.day = (uint8_t)st->wDay;
	m_ts.hour = (uint8_t)st->wHour;
	m_ts.minute = (uint8_t)st->wMinute;
	m_ts.second = (uint8_t)st->wSecond;
	m_ts.reserved = 0;
}
#else
/**
 * Sets the timestamp of the field with a UNIX timestamp in UTC.
 *
 * @param tm UNIX timestamp in UTC, as retrieved from POSIX time().
 */
void DateField::SetTimestamp(time_t tm) {
	struct tm *uts = gmtime(&tm);
	m_ts.year = (uint16_t)(1900 + uts->tm_year);
	m_ts.month = (uint8_t)(uts->tm_mon + 1);
	m_ts.day = (uint8_t)uts->tm_mday;
	m_ts.hour = (uint8_t)uts->tm_hour;
	m_ts.minute = (uint8_t)uts->tm_min;
	m_ts.second = (uint8_t)uts->tm_sec;
	m_ts.reserved = 0;
}
#endif // _WIN32

/*
 * +===========================================================================+
 * |                                                                           |
 * |                                Overrides                                  |
 * |                                                                           |
 * +===========================================================================+
 */

uint16_t DateField::FieldLength() const {
	return Field::FieldLength() + sizeof(timestamp_t);
}

uint8_t DateField::ReadField(FHND hFile, size_t *bytes) {
	DWORD dwRead = 0;

	// Read the field's base.
	uint8_t depth = Field::ReadField(hFile, bytes);
	if (BolotaHasError)
		return BOLOTA_ERR_UINT8;

	// Read the timestamp.
	if (!FileUtils::Read(hFile, &this->m_ts, sizeof(timestamp_t), &dwRead)) {
		ThrowError(new ReadError(hFile, *bytes, true));
		return BOLOTA_ERR_UINT8;
	}
	*bytes += dwRead;

	return depth;
}

size_t DateField::Write(FHND hFile) const {
	// Write base of the field.
	size_t ulBytes = Field::Write(hFile);
	DWORD dwWritten = 0;

	// Check for errors in the base operation.
	if (BolotaHasError)
		return BOLOTA_ERR_SIZET;

	// Write the timestamp.
	if (!FileUtils::Write(hFile, &m_ts, sizeof(timestamp_t), &dwWritten)) {
		ThrowError(new WriteError(hFile, ulBytes, true));
		return BOLOTA_ERR_SIZET;
	}
	ulBytes += dwWritten;

	return ulBytes;
}
