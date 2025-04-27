/**
 * winfile.h
 * A shim for a bunch of very common file operations and stuff in Windows.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_SHIM_WINFILE_H
#define _BOLOTA_SHIM_WINFILE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "tchar.h"
#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * File handle pointer.
 */
typedef FILE* HANDLE;

/**
 * Representation of an invalid handle.
 */
#define INVALID_HANDLE_VALUE (HANDLE)((size_t)-1)

/* All the definitions for CreateFile */
#define GENERIC_READ "rb"
#define GENERIC_WRITE "wb"
#define FILE_SHARE_READ 0
#define OPEN_EXISTING 0
#define CREATE_ALWAYS 0
#define FILE_ATTRIBUTE_NORMAL 0
#define FILE_ATTRIBUTE_ARCHIVE 0

/**
 * Closes a file handle.
 *
 * @param hObject File handle to be closed.
 *
 * @return TRUE if the operation was successful.
 */
bool CloseHandle(FILE* hObject) {
	return fclose(hObject) == 0;
}

/**
 * Opens a file handle.
 *
 * @param lpFileName            Location of the file.
 * @param dwDesiredAccess       Read or write?
 * @param dwShareMode           Ignored.
 * @param lpSecurityAttributes  Ignored.
 * @param dwCreationDisposition Ignored.
 * @param dwFlagsAndAttributes  Ignored.
 * @param hTemplateFile         Ignored.
 *
 * @return File handle or INVALID_HANDLE_VALUE if something went wrong.
 */
HANDLE CreateFile(LPCTSTR lpFileName, LPCTSTR dwDesiredAccess,
				  uint8_t dwShareMode, DWORD* lpSecurityAttributes,
				  uint8_t dwCreationDisposition, uint8_t dwFlagsAndAttributes,
				  HANDLE hTemplateFile) {
	FILE *fh = fopen(lpFileName, dwDesiredAccess);
	if (fh == NULL)
		return INVALID_HANDLE_VALUE;

	return fh;
}

/**
 * Reads some bytes from a binary file.
 *
 * @param hFile                File handle.
 * @param lpBuffer             Pre-allocated buffer to store the read bytes.
 * @param nNumberOfBytesToRead Number of bytes to read.
 * @param lpNumberOfBytesRead  Actual number of bytes read.
 * @param lpOverlapped         Ignored.
 *
 * @return TRUE on success, FALSE otherwise.
 */
bool ReadFile(HANDLE hFile, void* lpBuffer, size_t nNumberOfBytesToRead,
			  size_t* lpNumberOfBytesRead, void* lpOverlapped) {
	size_t readlen = fread(lpBuffer, sizeof(uint8_t), nNumberOfBytesToRead,
		hFile);
	if (lpNumberOfBytesRead != NULL)
		*lpNumberOfBytesRead = readlen;

	// Check for errors.
	if ((readlen < nNumberOfBytesToRead) && (ferror(hFile) != 0)) {
		clearerr(hFile);
		return false;
	}

	return true;
}

/**
 * Writes some bytes to a binary file.
 *
 * @param hFile                  File handle.
 * @param lpBuffer               Buffer to get data to write.
 * @param nNumberOfBytesToWrite  Number of bytes to write from the buffer.
 * @param lpNumberOfBytesWritten Pointer to the number of bytes written.
 * @param lpOverlapped           Ignored.
 *
 * @return TRUE on success, FALSE otherwise.
 */
bool WriteFile(HANDLE hFile, const void* lpBuffer, size_t nNumberOfBytesToWrite,
			   size_t* lpNumberOfBytesWritten, void* lpOverlapped) {
	size_t writelen = fwrite(lpBuffer, sizeof(uint8_t), nNumberOfBytesToWrite,
		hFile);
	if (lpNumberOfBytesWritten != NULL)
		*lpNumberOfBytesWritten = writelen;

	// Check for errors.
	if ((writelen < nNumberOfBytesToWrite) && (ferror(hFile) != 0)) {
		clearerr(hFile);
		return false;
	}

	return true;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _BOLOTA_SHIM_WINFILE_H
