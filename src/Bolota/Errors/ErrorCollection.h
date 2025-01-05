/**
 * ErrorCollection.h
 * A collection of error classes related to the handling of Bolota documents and
 * fields.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_ERRORS_ERRORCOLLECTION_H
#define _BOLOTA_ERRORS_ERRORCOLLECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
	#include <windows.h>
#endif // _WIN32

#include "SystemError.h"
#include "../Field.h"

namespace Bolota {
	/**
	 * Error that is thrown whenever there's an inconsistency between related
	 * fields.
	 */
	class ConsistencyError : public Error {
	public:
		Field *fieldReference;
		Field *fieldExpected;
		Field *fieldFound;
		tstring strRelationship;

		ConsistencyError(Field *reference, Field *expected, Field *found,
						 const TCHAR *relationship) : Error() {
			// Set class properties.
			fieldReference = reference;
			fieldExpected = expected;
			fieldFound = found;
			strRelationship = relationship;

			// Create a temporary buffer for building up a complex string.
			TCHAR szBuffer[255];
			szBuffer[254] = _T('\0');

			// Build up our message string.
			tstring strMessage = _T("An inconsistency was found between ")
				_T("fields ");
			_sntprintf(szBuffer, 254, _T("%p (reference), %p (expected), and ")
				_T("%p (found) in relation to their %s property.") LINENDT
				LINENDT, reference, expected, found, relationship);
			strMessage += szBuffer;
			_sntprintf(szBuffer, 254, _T("Reference (%p): %s") LINENDT,
				reference, reference->Text()->GetNativeString());
			strMessage += szBuffer;
			_sntprintf(szBuffer, 254, _T("%s Expected (%p): %s") LINENDT,
				relationship, expected, (expected == NULL) ? NULL :
				expected->Text()->GetNativeString());
			strMessage += szBuffer;
			_sntprintf(szBuffer, 254, _T("%s Found (%p): %s"),
				relationship, found, (found == NULL) ? NULL :
				found->Text()->GetNativeString());
			strMessage += szBuffer;

			// Substitute the error message string.
			if (m_message)
				free(m_message);
			m_message = _tcsdup(strMessage.c_str());
		}
	};

	/**
	 * A generic error that can close file handles if needed.
	 */
	class FileHandleError : public SystemError {
	protected:
		HANDLE hFile;
		bool bHandleClosed;

		void Initialize(HANDLE hFile, bool bClose) {
			// Set internal variables.
			this->hFile = hFile;
			bHandleClosed = false;

			// Close the handle immediately if needed.
			if (bClose) {
				this->CloseHandle();
				this->hFile = NULL;
			}
		};

	public:
		FileHandleError(HANDLE hFile, bool bClose, const TCHAR *szMessage) :
			SystemError(szMessage) {
			this->Initialize(hFile, bClose);
		};
		FileHandleError(HANDLE hFile, bool bClose) :
			SystemError(_T("Unknown Bolota file handle error")) {
			this->Initialize(hFile, bClose);
		};

		void CloseHandle() {
			::CloseHandle(hFile);
			bHandleClosed = true;
		};

		bool IsHandleClosed() const {
			return bHandleClosed;
		}
	};

	/**
	 * Thrown whenever the magic code of a file isn't what's expected.
	 */
	class InvalidMagic : public FileHandleError {
	public:
		InvalidMagic(HANDLE hFile) : FileHandleError(hFile, true,
			_T("The file is not a valid Bolota document")) {};
	};

	/**
	 * Thrown whenever the version of a file is greater than we can support.
	 */
	class InvalidVersion : public FileHandleError {
	public:
		InvalidVersion(HANDLE hFile) : FileHandleError(hFile, true,
			_T("The document was saved with a newer version of the application ")
			_T("and may be incompatible with this one")) {};
	};

	/**
	 * Thrown whenever there's an error while trying to read a document.
	 */
	class ReadError : public FileHandleError {
	public:
		size_t ulPosition;

		ReadError(HANDLE hFile, bool bClose) :
			FileHandleError(hFile, bClose, _T("Failed to read file")) {};

		ReadError(HANDLE hFile, size_t ulPosition, bool bClose) :
			FileHandleError(hFile, bClose, _T("Failed to read file")) {
			// Get position index as string.
			TCHAR szIndex[20];
			_sntprintf(szIndex, 19, _T("%lu"), ulPosition);
			szIndex[19] = _T('\0');

			// Create the message.
			this->ulPosition = ulPosition;
			tstring strReadError = _T("Failed to read file at position ");
			strReadError += szIndex;
			RefreshMessage(strReadError.c_str());
		};
	};

	/**
	 * Thrown whenever there's an error while trying to write a document file.
	 */
	class WriteError : public FileHandleError {
	public:
		size_t ulPosition;

		WriteError(HANDLE hFile, bool bClose) :
			FileHandleError(hFile, bClose, _T("Failed to write file")) {
		};

		WriteError(HANDLE hFile, size_t ulPosition, bool bClose) :
			FileHandleError(hFile, bClose, _T("Failed to write file")) {
			// Get position index as string.
			TCHAR szIndex[20];
			_sntprintf(szIndex, 19, _T("%lu"), ulPosition);
			szIndex[19] = _T('\0');

			// Create the message.
			this->ulPosition = ulPosition;
			tstring strReadError = _T("Failed to write file at position ");
			strReadError += szIndex;
			RefreshMessage(strReadError.c_str());
		};
	};

	/**
	 * Thrown whenever we encounter an unknown field type while trying to read a
	 * document.
	 */
	class UnknownFieldType : public ReadError {
	protected:
		bolota_type_t m_type;

	public:
		UnknownFieldType(HANDLE hFile, size_t ulPosition, bool bClose,
						 bolota_type_t type) :
		ReadError(hFile, ulPosition, bClose) {
			// Set the type variable.
			m_type = type;

			// Get type number as string.
			TCHAR szTypeNum[4];
			_sntprintf(szTypeNum, 3, _T("%u"), type);
			szTypeNum[3] = _T('\0');

			// Append more information to our message.
			tstring strReadError(m_message);
			strReadError += _T(". Encountered an unknown field type ");
			strReadError += szTypeNum;
			strReadError += _T(" '");
#ifdef UNICODE
			strReadError += (wchar_t)(L'\0' | m_type);
#else
			strReadError += static_cast<char>(m_type);
#endif // UNICODE
			strReadError += _T("'");
			RefreshMessage(strReadError.c_str());
		};
	};
}

#endif // _BOLOTA_ERRORS_ERRORCOLLECTION_H
