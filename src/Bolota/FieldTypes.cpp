/**
 * FieldTypes.cpp
 * Provides a convenient way of enumerating and referencing all available field
 * types in the application.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "FieldTypes.h"

using namespace Bolota;

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
	// Clear the list
	for (uint8_t i = 0; i < m_list.size(); ++i)
		delete m_list[i];

	// Invalidate our singleton instance.
	Instance(true);
}

/**
 * Internal global singleton instance allocator and invalidator.
 *
 * @param invalidate Should we invalidate our global instance?
 *
 * @return Global singleton instance.
 */
FieldTypeList* FieldTypeList::Instance(bool invalidate) {
	static FieldTypeList* list;

	// Should we invalidate?
	if (invalidate) {
		list = NULL;
		return NULL;
	}

	// Allocate our global instance if needed.
	if (list == NULL)
		list = new FieldTypeList();

	return list;
}

/**
 * Gets the global instance of the field type list singleton.
 *
 * @return Global field type list object.
 */
FieldTypeList* FieldTypeList::Instance() {
	return Instance(false);
}

/**
 * Gets the list of field types associated with the object.
 * 
 * @return Vector of field types available.
 */
std::vector<FieldType*>& FieldTypeList::List() {
	return FieldTypeList::Instance()->m_list;
}
