/**
 * FileUtils.h
 * A shim for a bunch of very common file operations.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_UTILS_FILEUTILS_H
#define _BOLOTA_UTILS_FILEUTILS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <tchar.h>
#ifdef _WIN32
	#include <windows.h>
#endif // _WIN32

/**
 * File handle pointer.
 */
#ifdef _WIN32
typedef HANDLE FHND;
#else
typedef FILE* FHND;
#endif

/**
 * Representation of an invalid handle.
 */
#ifndef _WIN32
	#define INVALID_HANDLE_VALUE (FHND)((size_t)-1)
#endif

namespace FileUtils {

FHND Open(LPCTSTR szFilename, bool bWrite, bool bBinary);
bool Close(FHND hFile);
bool Read(FHND hFile, void* lpBuffer, size_t nBytesToRead,
	size_t* lpnBytesRead);
bool Write(FHND hFile, const void* lpBuffer, size_t nBytesToWrite,
	size_t* lpnBytesWritten);

}

#endif // _BOLOTA_UTILS_FILEUTILS_H
