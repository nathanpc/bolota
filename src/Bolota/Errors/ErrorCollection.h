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
			FileHandleError(hFile, bClose, _T("Failed to read file")) {
			this->ulPosition = -1L;
		};

		ReadError(HANDLE hFile, size_t ulPosition, bool bClose) :
			FileHandleError(hFile, bClose, _T("Failed to read file")) {
			// Get position index as string.
			TCHAR szIndex[20];
#if _MSC_VER >= 1200
			_sntprintf(szIndex, 19, _T("%zu"), ulPosition);
#else
			_sntprintf(szIndex, 19, _T("%lu"), ulPosition);
#endif // _MSC_VER >= 1200
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
			this->ulPosition = -1L;
		};

		WriteError(HANDLE hFile, size_t ulPosition, bool bClose) :
			FileHandleError(hFile, bClose, _T("Failed to write file")) {
			// Get position index as string.
			TCHAR szIndex[20];
#if _MSC_VER >= 1200
			_sntprintf(szIndex, 19, _T("%zu"), ulPosition);
#else
			_sntprintf(szIndex, 19, _T("%lu"), ulPosition);
#endif // _MSC_VER >= 1200
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
