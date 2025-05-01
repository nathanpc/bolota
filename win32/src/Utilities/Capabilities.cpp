/**
 * Capabilities.cpp
 * Checks the Windows version to enable or disable features during runtime.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Capabilities.h"

#include <windows.h>

using namespace Capability;

// Instance Windows version structure.
static OSVERSIONINFO osvi = {0};

/**
 * Gets the current version information about the running system and stores it
 * in the internal OSVERSIONINFO structure.
 */
void GetOSVersionInfo() {
#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 28159)
	// Get the version info if we haven't already.
	if (osvi.dwOSVersionInfoSize == 0) {
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
	}
#pragma warning(pop)
}

/**
 * Checks wheter the current running system version is Windows Vista or later.
 * 
 * @return TRUE if the running system version is at least Windows Vista.
 */
bool Capability::AtLeastWindowsVista() {
	GetOSVersionInfo();
	return osvi.dwMajorVersion >= 6;
}

/**
 * Checks wheter the current running system version is Windows XP or later.
 * 
 * @return TRUE if the running system version is at least Windows XP.
 */
bool Capability::AtLeastWindowsXP() {
	GetOSVersionInfo();
	return (osvi.dwMajorVersion >= 5) ||
		((osvi.dwMajorVersion >= 5) && (osvi.dwMinorVersion >= 1));
}
