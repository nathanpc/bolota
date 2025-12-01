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
#include <stdint.h>
#include <tchar.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <stdbool.h>
#endif // _WIN32

/**
 * File handle pointer.
 */
#ifdef _WIN32
typedef HANDLE FHND;
#else
typedef FILE* FHND;
#endif // _WIN32

/**
 * File size abstraction.
 */
#ifdef _WIN32
typedef DWORD fsize_t;
#else
typedef size_t fsize_t;
#endif // _WIN32

/**
 * Representation of an invalid handle.
 */
#ifndef _WIN32
	#define INVALID_HANDLE_VALUE (FHND)((size_t)-1)
#endif

namespace FileUtils {

FHND Open(LPCTSTR szFilename, bool bWrite, bool bBinary);
bool Close(FHND hFile);
bool Read(FHND hFile, void* lpBuffer, fsize_t nBytesToRead,
	fsize_t* lpnBytesRead);
bool Write(FHND hFile, const void* lpBuffer, fsize_t nBytesToWrite,
	fsize_t* lpnBytesWritten);

}

#endif // _BOLOTA_UTILS_FILEUTILS_H
