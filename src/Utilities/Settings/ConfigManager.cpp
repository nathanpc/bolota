/**
 * ConfigManager.cpp
 * Manages the application's configuration options.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "ConfigManager.h"

#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

#include "../../stdafx.h"
#include "../../Bolota/Errors/SystemError.h"
#include "../Capabilities.h"
#include "Setting.h"

using namespace Settings;

// File association definitions.
#define BOLOTA_PROGID   _T("Bolota.Document.1")
#define BOLOTA_MIMETYPE _T("application/x.bolota.document")

// Helpers for error checking.
#define ThrowErrorRegOpen(name, code) \
	if (dwResult != ERROR_SUCCESS) { \
		ThrowError(new SystemError(EMSG("Failed to open registry ") _T(name) \
			_T(" subkey for writing"), code)); \
		return false; \
	}
#define ThrowErrorRegWrite(name, code) \
	if (dwResult != ERROR_SUCCESS) { \
		ThrowError(new SystemError(EMSG("Failed to write subkey") _T(name) \
			_T(" value to registry"), code)); \
		return false; \
	}

/**
 * Initializes the configuration manager.
 */
ConfigManager::ConfigManager() {
	// Initialize internals.
	hInst = NULL;
	m_settings = (BaseSetting**)malloc(BOLOTA_SETTINGS_NUM *
		sizeof(BaseSetting*));
	if (m_settings == NULL) {
		ThrowError(EMSG("Failed to allocate settings array"));
		return;
	}

	// Populate the settings array.
	m_settings[WindowWidth] = new Setting<DWORD>(_T("WindowWidth"),
		_T("Window width"), REG_DWORD, (DWORD)600);
	m_settings[WindowHeight] = new Setting<DWORD>(_T("WindowHeight"),
		_T("Window height"), REG_DWORD, (DWORD)400);
	m_settings[TreeViewStyleFlags] = new Setting<DWORD>(
		_T("TreeViewStyleFlags"), _T("TreeView style flags"), REG_DWORD,
		(DWORD)(TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS));
}

/**
 * Cleans up all the memory allocated by the configuration manager.
 */
ConfigManager::~ConfigManager() {
	// Free our settings array.
	if (m_settings) {
		for (uint8_t i = 0; i < BOLOTA_SETTINGS_NUM; ++i)
			delete m_settings[i];
		free(m_settings);
		m_settings = NULL;
	}
}

/**
 * Gets the global instance of the configuration manager singleton.
 *
 * @return Global configuration manager object.
 */
ConfigManager* ConfigManager::Instance() {
	static ConfigManager* manager;
	if (manager == NULL)
		manager = new ConfigManager();

	return manager;
}

/**
 * Associates our file extension with the application that's currently running.
 * 
 * @return TRUE if the operation was successful. FALSE otherwise.
 */
bool ConfigManager::AssociateFileExtension() const {
	HKEY hkeyRoot = HKEY_CLASSES_ROOT;
	TCHAR szClassRoot[18] = _T("");
	TCHAR szKeyPath[200];
	TCHAR szValue[200];
	TCHAR szExePath[MAX_PATH];
	DWORD dwDisposition;
	DWORD dwResult;
	HKEY hKey;
	DWORD dwLength;

#ifndef UNDER_CE
	// Use the user registry keys introduced in Windows Vista.
	if (Capability::AtLeastWindowsVista()) {
		hkeyRoot = HKEY_CURRENT_USER;
		_tcscpy(szClassRoot, _T("Software\\Classes\\"));
	}
#endif // !UNDER_CE

	// Get the path to ourselves.
	GetModuleFileName(NULL, szExePath, MAX_PATH);
	
	/* HKEY_CLASSES_ROOT\Bolota.Document.1\ */
	_stprintf(szKeyPath, _T("%s") BOLOTA_PROGID, szClassRoot);
	dwResult = RegCreateKeyEx(hkeyRoot, szKeyPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
	ThrowErrorRegOpen("PROGID", dwResult);
	LoadString(hInst, IDS_DOC_DESC, szValue, 200);
	dwResult = RegSetValueEx(hKey, _T(""), 0, REG_SZ, (LPBYTE)szValue,
		(DWORD)((_tcslen(szValue) + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite("PROGID", dwResult);

	/* HKEY_CLASSES_ROOT\Bolota.Document.1\FriendlyTypeName */
	dwLength = _stprintf(szValue, _T("@%s,-%d"), szExePath, IDS_DOC_DESC);
	dwResult = RegSetValueEx(hKey, _T("FriendlyTypeName"), 0, REG_SZ,
		(LPBYTE)szValue, (DWORD)((dwLength + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite("PROGID\\FriendlyTypeName", dwResult);
	RegCloseKey(hKey);

	/* HKEY_CLASSES_ROOT\Bolota.Document.1\CurVer */
	_stprintf(szKeyPath, _T("%s") BOLOTA_PROGID _T("\\CurVer"), szClassRoot);
	dwResult = RegCreateKeyEx(hkeyRoot, szKeyPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
	ThrowErrorRegOpen("PROGID\\CurVer", dwResult);
	dwLength = _stprintf(szValue, BOLOTA_PROGID);
	dwResult = RegSetValueEx(hKey, _T(""), 0, REG_SZ, (LPBYTE)szValue,
		(DWORD)((dwLength + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite("PROGID\\CurVer", dwResult);
	RegCloseKey(hKey);

	/* HKEY_CLASSES_ROOT\Bolota.Document.1\DefaultIcon */
	_stprintf(szKeyPath, _T("%s") BOLOTA_PROGID _T("\\DefaultIcon"),
		szClassRoot);
	dwResult = RegCreateKeyEx(hkeyRoot, szKeyPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
	ThrowErrorRegOpen("PROGID\\DefaultIcon", dwResult);
	dwLength = _stprintf(szValue, _T("%s,-%d"), szExePath, IDI_BOLOTA);
	dwResult = RegSetValueEx(hKey, _T(""), 0, REG_SZ, (LPBYTE)szValue,
		(DWORD)((dwLength + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite("PROGID\\DefaultIcon", dwResult);
	RegCloseKey(hKey);

	/* HKEY_CLASSES_ROOT\Bolota.Document.1\shell\open\command */
	_stprintf(szKeyPath, _T("%s") BOLOTA_PROGID _T("\\shell\\open\\command"),
		szClassRoot);
	dwResult = RegCreateKeyEx(hkeyRoot, szKeyPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
	ThrowErrorRegOpen("PROGID\\shell\\open\\command", dwResult);
#ifndef UNDER_CE
	dwLength = _stprintf(szValue, _T("\"%s\" \"%%1\""), szExePath);
#else
	dwLength = _stprintf(szValue, _T("\"%s\" %%1"), szExePath);
#endif // !UNDER_CE
	dwResult = RegSetValueEx(hKey, _T(""), 0, REG_SZ, (LPBYTE)szValue,
		(DWORD)((dwLength + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite("PROGID\\shell\\open\\command", dwResult);
	RegCloseKey(hKey);

	/* HKEY_CLASSES_ROOT\.bol\ */
	_stprintf(szKeyPath, _T("%s.bol"), szClassRoot);
	dwResult = RegCreateKeyEx(hkeyRoot, szKeyPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
	ThrowErrorRegOpen(".bol", dwResult);
	dwLength = _stprintf(szValue, BOLOTA_PROGID);
	dwResult = RegSetValueEx(hKey, _T(""), 0, REG_SZ, (LPBYTE)szValue,
		(DWORD)((dwLength + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite(".bol", dwResult);

	/* HKEY_CLASSES_ROOT\.bol\PerceivedType */
	dwLength = _stprintf(szValue, _T("Document"));
	dwResult = RegSetValueEx(hKey, _T("PerceivedType"), 0, REG_SZ,
		(LPBYTE)szValue, (DWORD)((dwLength + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite(".bol\\PerceivedType", dwResult);

	/* HKEY_CLASSES_ROOT\.bol\Content Type */
	dwLength = _stprintf(szValue, BOLOTA_MIMETYPE);
	dwResult = RegSetValueEx(hKey, _T("Content Type"), 0, REG_SZ,
		(LPBYTE)szValue, (DWORD)((dwLength + 1) * sizeof(TCHAR)));
	ThrowErrorRegWrite(".bol\\Content Type", dwResult);
	RegCloseKey(hKey);

#ifndef UNDER_CE
	// Ensure the shell gets notified of this change.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
#endif // !UNDER_CE

	// Notify the user about the association.
	MsgBoxInfo(NULL, _T("File Association Changed"),
		_T("Successfully associated *.bol files with the application."));

	return true;
}

/**
 * Gets the setting at the specified index.
 *
 * @param index Index where the desired setting object is.
 *
 * @return Desired setting object.
 */
BaseSetting* ConfigManager::Get(SettingIndex index) const {
	return m_settings[index];
};

/**
 * Sets the HINSTANCE associated with the manager.
 * 
 * @param hInst Application's HINSTANCE.
 */
void ConfigManager::SetHInstance(HINSTANCE hInst) {
	this->hInst = hInst;
}
