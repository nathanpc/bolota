/**
 * ConsistencyError.h
 * Error that is thrown whenever there's an inconsistency between related
 * fields.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_ERRORS_CONSISTENCYERROR_H
#define _BOLOTA_ERRORS_CONSISTENCYERROR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
	#include "../../stdafx.h"
#endif // _WIN32

#include "Error.h"
#include "../Field.h"

namespace Bolota {

	/**
	 * Error that is thrown whenever there's an inconsistency between related
	 * fields.
	 */
	class ConsistencyError : public Error {
	public:
		Field* fieldReference;
		Field* fieldExpected;
		Field* fieldFound;
		tstring strRelationship;

		ConsistencyError(Field* reference, Field* expected, Field* found,
			const TCHAR* relationship);
	};

};

#endif // _BOLOTA_ERRORS_CONSISTENCYERROR_H
