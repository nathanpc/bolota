/**
 * FieldTypes.cpp
 * Provides a convenient way of enumerating and referencing all available field
 * types in the application.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "FieldTypes.h"

using namespace Bolota;

// Singleton field types list instance.
FieldTypeList* FieldTypeList::m_inst = FieldTypeList::Instance();

/**
 * Internal singleton constructor.
 */
FieldTypeList::FieldTypeList() {
	m_list.push_back(new FieldType(BOLOTA_TYPE_TEXT, _T("Text")));
	m_list.push_back(new FieldType(BOLOTA_TYPE_ICON, _T("Icon with Text")));
	m_list.push_back(new FieldType(BOLOTA_TYPE_DATE, _T("Date & Time")));
}

/**
 * Clears up any memory allocated by the object and ensures its singleton
 * instance is NULLed.
 */
FieldTypeList::~FieldTypeList() {
	for (uint8_t i = 0; i < m_list.size(); ++i)
		delete m_list[i];
	m_inst = NULL;
}

/**
 * Gets the global instance of the field type list singleton.
 *
 * @return Global field type list object.
 */
FieldTypeList* FieldTypeList::Instance() {
	if (m_inst == NULL)
		m_inst = new FieldTypeList();

	return m_inst;
}

/**
 * Gets the list of field types associated with the object.
 * 
 * @return Vector of field types available.
 */
std::vector<FieldType*>& FieldTypeList::List() {
	return FieldTypeList::Instance()->m_list;
}
