/**
 * Exceptions.h
 * A collection of exceptions related to the handling of Bolota documents and
 * fields.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_EXCEPTIONS_H
#define _BOLOTA_EXCEPTIONS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdexcept>
#include <string>

#include "../Exceptions/SystemException.h"

namespace Bolota {
	/**
	 * A generic exception that can close file handles if needed.
	 */
	class FileHandleException : public SystemException {
	protected:
		HANDLE hFile;
		bool bHandleClosed;

		void Initialize(HANDLE hFile, bool bClose) {
			// Initialize the base class.
			this->SystemException::Initialize(NULL);

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
		FileHandleException(HANDLE hFile, bool bClose, const char *szMessage) :
			SystemException(szMessage) {
				Initialize(hFile, bClose);
		};
		FileHandleException(HANDLE hFile, bool bClose) :
			SystemException("Unknown Bolota file handle exception") {
				Initialize(hFile, bClose);
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
	class InvalidMagic : public FileHandleException {
	public:
		InvalidMagic(HANDLE hFile) :
			FileHandleException(hFile, true, "The file is not a valid Bolota "
			"document") {};
	};

	/**
	 * Thrown whenever the version of a file is greater than we can support.
	 */
	class InvalidVersion : public FileHandleException {
	public:
		InvalidVersion(HANDLE hFile) :
			FileHandleException(hFile, true, "The document was saved with a "
			"newer version of the application and may be incompatible with "
			"this one") {};
	};

	/**
	 * Thrown whenever there's an error while trying to read a document.
	 */
	class ReadError : public FileHandleException {
	protected:
		std::string m_strReadError;

	public:
		size_t ulPosition;

		ReadError(HANDLE hFile, bool bClose) :
			FileHandleException(hFile, bClose, "Failed to read file") {};
		ReadError(HANDLE hFile, size_t ulPosition, bool bClose) :
			FileHandleException(hFile, bClose, "Failed to read file") {
				Initialize(ulPosition);
		};

		virtual void Initialize(size_t ulPosition) {
			// Get position index as string.
			char szIndex[20];
			_snprintf(szIndex, 19, "%lu", ulPosition);
			szIndex[19] = '\0';

			// Create the message.
			this->ulPosition = ulPosition;
			m_strReadError += "Failed to read file at position ";
			m_strReadError += szIndex;
			RefreshMessage(m_strReadError.c_str());
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
			bolota_type_t type) : ReadError(hFile, ulPosition, bClose) {
				Initialize(type);
		};

		void Initialize(bolota_type_t type) {
			// Set the type variable.
			m_type = type;

			// Get type number as string.
			char szTypeNum[4];
			_snprintf(szTypeNum, 3, "%u", type);
			szTypeNum[3] = '\0';

			// Append more information to our message.
			m_strReadError += ". Encountered an unknown field type ";
			m_strReadError += szTypeNum;
			m_strReadError += " '";
			m_strReadError += static_cast<char>(m_type);
			m_strReadError += "'";
			RefreshMessage(m_strReadError.c_str());
		};
	};
}

#endif // _BOLOTA_EXCEPTIONS_H
