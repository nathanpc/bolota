/**
 * Error.h
 * Error handling and reporting class.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_ERRORS_ERROR_H
#define _BOLOTA_ERRORS_ERROR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>
#ifdef _WIN32
	#include <windows.h>
#endif // _WIN32

/**
 * Convinience macro for throwing errors.
 */
#define ThrowError(err) Bolota::Error::Throw((err))

#ifdef DEBUG
	#define STRINGIZE(x) STRINGIZE_WRAPPER(x)
	#define STRINGIZE_WRAPPER(x) #x
	#define EMSG(msg) _T(" [") _T(__FILE__) _T(":") _T(STRINGIZE(__LINE__)) _T("]") _T(msg)
#else
	#define EMSG(msg) _T(msg)
#endif // DEBUG

/**
 * Convinience definitions for error returns.
 */
#define BOLOTA_ERR_NULL  (NULL)
#define BOLOTA_ERR_UINT8 ((UINT8)-1)
#define BOLOTA_ERR_SIZET ((size_t)-1L)

namespace Bolota {

	/**
	 * Error handling and reporting class.
	 */
	class Error {
	protected:
		Error *m_prev;
		TCHAR *m_message;

	public:
		// Constructors and destructors.
		Error();
		Error(const TCHAR *message);
		Error(Error *prev, const TCHAR *message);
		virtual ~Error();
		static Error* Throw(const TCHAR *message);
		static Error* Throw(Error *error);

		// Getters and setters.
		const TCHAR* Message();

		// Stack operations.
		static bool HasError();
		Error* Pop();
		static void Clear();
	};

	/**
	 * Global error stack definition.
	 */
	static Error* ErrorStack = NULL;

}

#endif // _BOLOTA_ERRORS_ERROR_H
