/**
 * Setting.cpp
 * Abstraction over a single setting in the configuration manager.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Setting.h"

using namespace Settings;

#include <stdlib.h>

#include "../../Bolota/Errors/Error.h"

/**
 * Initializes the base settings object.
 * 
 * @param key         Key used when storing the setting.
 * @param description Human-friendly description of the setting.
 */
BaseSetting::BaseSetting(const TCHAR* key, const TCHAR* description) {
	this->m_key = _tcsdup(key);
	this->m_description = _tcsdup(description);
}

/**
 * Frees up everything related to the base settings.
 */
BaseSetting::~BaseSetting() {
	// Free the key.
	if (m_key)
		free(m_key);
	m_key = NULL;

	// Free the description.
	if (m_description)
		free(m_description);
	m_description = NULL;
}

/**
 * Loads a setting from the Windows registry if it exists or use a default
 * value.
 * 
 * @return ERROR_SUCCESS if a value was loaded correctly, ERROR_FILE_NOT_FOUND
 *         the key doesn't exist. Everything should be treated as an error.
 */
template<typename T>
LSTATUS Setting<T>::LoadValue(LPBYTE lpValue, DWORD* lpdwLength) {
	HKEY hKey;

	// Open the registry key.
	LSTATUS lResult = RegOpenKeyEx(HKEY_CURRENT_USER, BOLOTA_REG_KEY, 0,
		KEY_READ, &hKey);
	if (lResult != ERROR_SUCCESS) {
		// Key wasn't found. We should fallback to the default value.
		if (lResult == ERROR_FILE_NOT_FOUND)
			return lResult;

		// Looks like bad things happened.
		ThrowError(EMSG("Failed to open registry subkey for reading"));
		return lResult;
	}

	// Get value from registry.
	lResult = RegQueryValueEx(hKey, m_key, NULL, NULL, lpValue, lpdwLength);
	if (lResult != ERROR_SUCCESS) {
		// Key wasn't found. We should fallback to the default value.
		if (lResult == ERROR_FILE_NOT_FOUND)
			return lResult;

		ThrowError(EMSG("Failed to get setting value from registry"));
	}

	// Close the key handle.
	RegCloseKey(hKey);

	return lResult;
}

template<>
LSTATUS Setting<DWORD>::Load() {
	DWORD dwLength = 0;
	return LoadValue((LPBYTE)&m_value, &dwLength);
}

/**
 * Saves a setting value to the Windows registry.
 * 
 * @param dwType   Type of registry value to save to.
 * @param dwLength Size of the data we need to write.
 *
 * @return TRUE if the value was saved, FALSE if an error occurred.
 */
template<typename T>
bool Setting<T>::Save(DWORD dwType, DWORD dwLength, LPCBYTE lpData) {
	HKEY hKey;

	// Open the registry key.
	LSTATUS lResult = RegCreateKeyEx(HKEY_CURRENT_USER, BOLOTA_REG_KEY, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
	if (lResult != ERROR_SUCCESS) {
		// Looks like bad things happened.
		ThrowError(EMSG("Failed to open registry subkey for writing"));
		return false;
	}

	// Get value from registry.
	lResult = RegSetValueEx(hKey, m_key, 0, dwType, lpData, dwLength);
	if (lResult != ERROR_SUCCESS)
		ThrowError(EMSG("Failed to set setting value to registry"));

	// Close the key handle.
	RegCloseKey(hKey);

	return lResult == ERROR_SUCCESS;
}

/**
 * Saves a setting value to the Windows registry.
 *
 * @param dwType   Type of registry value to save to.
 * @param dwLength Size of the data we need to write.
 *
 * @return TRUE if the value was saved, FALSE if an error occurred.
 */
template<typename T>
bool Setting<T>::Save(DWORD dwType, DWORD dwLength) {
	return Save(dwType, dwLength, (LPBYTE)&m_value);
}

/**
 * Gets the value associated with the setting.
 * 
 * @return Value of the setting.
 */
template<typename T>
T Setting<T>::Value() const {
	return m_value;
}

/**
 * Sets the value associated with the setting.
 *
 * @param value New value of the setting.
 */
template<typename T>
void Setting<T>::SetValue(T value) {
	m_value = value;
}

/**
 * Gets the key used when storing the setting
 * 
 * @return Setting key used for storage.
 */
const TCHAR* BaseSetting::Key() const {
	return this->m_key;
}

/**
 * Gets the human-friendly description of the setting
 *
 * @return Human-friendly description.
 */
const TCHAR* BaseSetting::Description() const {
	return this->m_description;
}
