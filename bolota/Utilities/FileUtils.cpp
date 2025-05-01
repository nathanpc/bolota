/**
 * FileUtils.cpp
 * A shim for a bunch of very common file operations.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "FileUtils.h"

/**
 * Opens a file handle.
 *
 * @param szFilename Location of the file to open.
 * @param bWrite     Should we open for writing?
 * @param bBinary    Is this a binary file?
 *
 * @return File handle or INVALID_HANDLE_VALUE if something went wrong.
 */
FHND FileUtils::Open(LPCTSTR szFilename, bool bWrite, bool bBinary) {
#ifdef _WIN32
	FHND hFile = INVALID_HANDLE_VALUE;

	if (bWrite) {
		hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	} else {
		CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	}
#else
	// Build up the file operation flags.
	char flags[3];
	flags[0] = (bWrite) ? 'w' : 'r';
	flags[1] = (bBinary) ? 'b' : '\0';
	flags[2] = '\0';

	// Open the file.
	FHND hFile = fopen(szFilename, flags);
	if (hFile == NULL)
		return INVALID_HANDLE_VALUE;
#endif // _WIN32

	return hFile;
}

/**
 * Closes a file handle.
 *
 * @param hFile File handle to be closed.
 *
 * @return TRUE if the operation was successful.
 */
bool FileUtils::Close(FHND hFile) {
#ifdef _WIN32
	return CloseHandle(hFile);
#else
	return fclose(hFile) == 0;
#endif // _WIN32
}

/**
 * Reads some bytes from a binary file.
 *
 * @param hFile        File handle to read from.
 * @param lpBuffer     Pre-allocated buffer to store the read bytes.
 * @param nBytesToRead Number of bytes to read.
 * @param lpnBytesRead Actual number of bytes read.
 *
 * @return TRUE on success, FALSE otherwise.
 */
bool FileUtils::Read(FHND hFile, void* lpBuffer, size_t nBytesToRead,
					 size_t* lpnBytesRead) {
#ifdef _WIN32
	return ReadFile(hFile, lpBuffer, nBytesToRead, lpnBytesRead, NULL);
#else
	// Read into buffer.
	size_t nLen = fread(lpBuffer, sizeof(uint8_t), nBytesToRead, hFile);
	if (lpnBytesRead != NULL)
		*lpnBytesRead = nLen;

	// Check for errors.
	if ((nLen < nBytesToRead) && (ferror(hFile) != 0)) {
		clearerr(hFile);
		return false;
	}

	return true;
#endif // _WIN32
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
bool FileUtils::Write(FHND hFile, const void* lpBuffer, size_t nBytesToWrite,
					  size_t* lpnBytesWritten) {
#ifdef _WIN32
	return WriteFile(hFile, lpBuffer, nBytesToWrite, lpnBytesWritten, NULL);
#else
	size_t nLen = fwrite(lpBuffer, sizeof(uint8_t), nBytesToWrite,
		hFile);
	if (lpnBytesWritten != NULL)
		*lpnBytesWritten = nLen;

	// Check for errors.
	if ((nLen < nBytesToWrite) && (ferror(hFile) != 0)) {
		clearerr(hFile);
		return false;
	}

	return true;
#endif // _WIN32
}
