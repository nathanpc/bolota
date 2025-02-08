/**
 * Capabilities.h
 * Checks the Windows version to enable or disable features during runtime.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UTILITIES_CAPABILITIES_H
#define _UTILITIES_CAPABILITIES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Capability {
	bool AtLeastWindowsVista();
	bool AtLeastWindowsXP();
}

#endif // _UTILITIES_CAPABILITIES_H
