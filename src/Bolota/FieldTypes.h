/**
 * FieldTypes.h
 * Provides a convenient way of enumerating and referencing all available field
 * types in the application.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_FIELDTYPES_H
#define _BOLOTA_FIELDTYPES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __cplusplus
#include <stdint.h>
#include <windows.h>
#include <vector>
#if _MSC_VER <= 1200
#include <newcpp.h>
#endif // _MSC_VER == 1200

#include "UString.h"

extern "C" {
#endif // __cplusplus

/**
 * An 8-bit value that determines the type of a field.
 */
typedef enum bolota_type_e {
	BOLOTA_TYPE_IGNORE =  0,
	BOLOTA_TYPE_TEXT   = 'T',
	BOLOTA_TYPE_DATE   = 'd',
	BOLOTA_TYPE_ICON   = 'I'
} bolota_type_t;

#ifdef __cplusplus
}

namespace Bolota {

	/**
	 * Detailed field type container.
	 */
	struct FieldType {
		bolota_type_t code;
		UString* name;

		FieldType(bolota_type_t code, const TCHAR* name) {
			this->code = code;
			this->name = new UString(name);
		};

		virtual ~FieldType() {
			delete name;
		};
	};

	/**
	 * Singleton container for a list of detailed field types.
	 */
	class FieldTypeList {
	private:
		std::vector<FieldType*> m_list;
		FieldTypeList();
		static FieldTypeList* Instance(bool invalidate);

		// Not implemented.
		FieldTypeList(FieldTypeList const&);
		void operator=(FieldTypeList const&);

	public:
		// Destructor
		virtual ~FieldTypeList();

		// Singleton instance.
		static FieldTypeList* Instance();
		static std::vector<FieldType*>& List();
	};

}

#endif // __cplusplus

#endif // _BOLOTA_FIELDTYPES_H
