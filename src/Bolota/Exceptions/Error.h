/**
 * Error.h
 * Exception type class for systems without exception handling.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_EXCEPTIONS_ERROR_H
#define _BOLOTA_EXCEPTIONS_ERROR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>

/**
 * No exception flag definition.
 */
#ifdef UNDER_CE
	#define NOEXCEPT 1
#endif // UNDER_CE

#ifdef _WIN32
	#include <windows.h>

	#if _MSC_VER <= 1200
		#include <newcpp.h>
	#endif // _MSC_VER == 1200
#endif // _WIN32

#include "../UString.h"

/**
 * Convinience macro for throwing exceptions.
 */
#ifdef NOEXCEPT
	#define THROW_WARN(EXCDECL) { (EXCDECL).ThrowWarning(); return; }
	#define THROW_WARN_R(EXCDECL, RET) { (EXCDECL).ThrowWarning(); return; return RET; }
	#define THROW_FATAL(EXCDECL) { (EXCDECL).ThrowFatal(); return; }
	#define THROW_FATAL_R(EXCDECL, RET) { (EXCDECL).ThrowFatal(); return RET; }
#else
	#define THROW_WARN(EXCDECL) { (EXCDECL).ThrowWarning(); }
	#define THROW_WARN_R(EXCDECL, RET) THROW_WARN(EXCDECL)
	#define THROW_FATAL(EXCDECL) { (EXCDECL).ThrowFatal() }
	#define THROW_FATAL_R(EXCDECL, RET) THROW_FATAL(EXCDECL)
#endif // NOEXCEPT

/**
 * Convinience definitions for error returns.
 */
#define BOLOTA_ERR_UINT8 ((UINT8)-1)

namespace Bolota {

	/**
	 * Exception type class for systems without exception handling.
	 */
	class Error {
	protected:
		UString *m_message;

	public:
		Error();
		Error(const char *message);
		Error(const wchar_t *message);
		virtual ~Error();

		UString* Message() const;
		virtual void Throw() const;

		Error& ThrowWarning() const;
		Error& ThrowFatal() const;
	};

}

#endif // _BOLOTA_EXCEPTIONS_ERROR_H
