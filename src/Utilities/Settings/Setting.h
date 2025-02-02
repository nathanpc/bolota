/**
 * Setting.h
 * Abstraction over a single setting in the configuration manager.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UTILITIES_SETTINGS_SETTING_H
#define _UTILITIES_SETTINGS_SETTING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

#include "../../Bolota/Errors/Error.h"

/**
 * Gets the subkey of the application.
 */
#define BOLOTA_REG_KEY _T("Software\\Bolota\\")

/**
 * Gets the entry name for a setting of the application.
 */
#define RegEntryName(name) (BOLOTA_REG_KEY _T(name))

namespace Settings {

	/**
	 * Base of every setting in the configuration manager without a value.
	 */
	class BaseSetting {
	protected:
		LPTSTR m_szKey;
		LPTSTR m_szDescription;
		DWORD  m_dwType;

	public:
		// Constructors and destructors.
		BaseSetting(LPCTSTR szKey, LPCTSTR szDescription, DWORD dwType);
		virtual ~BaseSetting();

		// Registry management.
		virtual DWORD Load();
		virtual bool Save();

		// Getters
		LPCTSTR Key() const;
		LPCTSTR Description() const;
		DWORD Type() const;
	};

	/**
	 * Abstraction over a single setting in the configuration manager.
	 */
	template <typename T>
	class Setting : public BaseSetting {
	protected:
		T m_tValue;

	public:

		// =====================================================================
		// =                                                                   =
		// =                    Constructors and Destructors                   =
		// =                                                                   =
		// =====================================================================

		/**
		 * Initializes a setting object and loads its value from the registry if
		 * available.
		 * 
		 * @param szKey         Key used when storing the setting.
		 * @param szDescription Human-friendly description of the setting.
		 * @param dwType        Windows registry value type.
		 * @param tDefault      Default value to use in case we can't load one.
		 */
		Setting(const TCHAR* szKey, const TCHAR* szDescription, DWORD dwType,
			T tDefault) : BaseSetting(szKey, szDescription, dwType) {
			if (Load() != ERROR_SUCCESS) { m_tValue = tDefault; }
		};

		// =====================================================================
		// =                                                                   =
		// =                        Registry Management                        =
		// =                                                                   =
		// =====================================================================

		/**
		 * Loads a setting from the Windows registry if it exists or use a
		 * default value.
		 * 
		 * @param lpValue    Pointer to where to store the retrieved value.
		 * @param lpdwLength Size of the buffer pointed by lpValue in bytes.
		 *
		 * @return ERROR_SUCCESS if a value was loaded correctly,
		 *         ERROR_FILE_NOT_FOUND the key doesn't exist. Everything else
		 *         should be treated as an error.
		 */
		LSTATUS Load(LPBYTE lpValue, DWORD* lpdwLength) {
			HKEY hKey;

			// Open the registry key.
			LSTATUS lResult = RegOpenKeyEx(HKEY_CURRENT_USER, BOLOTA_REG_KEY,
				0, KEY_READ, &hKey);
			if (lResult != ERROR_SUCCESS) {
				// Key wasn't found. We should fallback to the default value.
				if (lResult == ERROR_FILE_NOT_FOUND)
					return lResult;

				// Looks like bad things happened.
				ThrowError(EMSG("Failed to open registry subkey for ")
					_T("reading"));
				return lResult;
			}

			// Get value from registry.
			lResult = RegQueryValueEx(hKey, m_szKey, NULL, NULL, lpValue,
				lpdwLength);
			if (lResult != ERROR_SUCCESS) {
				// Key wasn't found. We should fallback to the default value.
				if (lResult == ERROR_FILE_NOT_FOUND)
					return lResult;

				ThrowError(EMSG("Failed to get setting value from ")
					_T("registry"));
			}

			// Close the key handle.
			RegCloseKey(hKey);

			return lResult;
		};

		/**
		 * Loads a setting from the Windows registry if it exists or use a
		 * default value.
		 * 
		 * @warning This method assumes the size of the value based on its type,
		 *          meaning all pointers will fail to be properly handled.
		 *
		 * @return ERROR_SUCCESS if a value was loaded correctly,
		 *         ERROR_FILE_NOT_FOUND the key doesn't exist. Everything else
		 *         should be treated as an error.
		 */
		LSTATUS Load() {
			DWORD dwLength = sizeof(m_tValue);
			return Load((LPBYTE)&m_tValue, &dwLength);
		}

		/**
		 * Saves a setting value to the Windows registry.
		 *
		 * @param dwLength Size of the data we need to write.
		 *
		 * @return TRUE if the value was saved, FALSE if an error occurred.
		 */
		bool Save(LPCBYTE lpData, DWORD dwLength) {
			HKEY hKey;

			// Open the registry key.
			LSTATUS lResult = RegCreateKeyEx(HKEY_CURRENT_USER, BOLOTA_REG_KEY,
				0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
			if (lResult != ERROR_SUCCESS) {
				// Looks like bad things happened.
				ThrowError(EMSG("Failed to open registry subkey for writing"));
				return false;
			}

			// Get value from registry.
			lResult = RegSetValueEx(hKey, m_szKey, 0, m_dwType, lpData,
				dwLength);
			if (lResult != ERROR_SUCCESS)
				ThrowError(EMSG("Failed to set setting value to registry"));

			// Close the key handle.
			RegCloseKey(hKey);

			return lResult == ERROR_SUCCESS;
		};

		/**
		 * Saves a setting value to the Windows registry.
		 * 
		 * @warning This method assumes the size of the value based on its type,
		 *          meaning all buffers will fail to be properly handled.
		 *
		 * @return TRUE if the value was saved, FALSE if an error occurred.
		 */
		bool Save() {
			return Save((LPBYTE)&m_tValue, sizeof(m_tValue));
		};

		// =====================================================================
		// =                                                                   =
		// =                        Getters and Setters                        =
		// =                                                                   =
		// =====================================================================

		/**
		 * Gets the value associated with the setting.
		 *
		 * @return Value of the setting.
		 */
		T Value() const {
			return m_tValue;
		}

		/**
		 * Sets the value associated with the setting.
		 *
		 * @param value New value of the setting.
		 * 
		 * @return Ourselves, for easily concatenating method calls.
		 */
		Setting<T>* SetValue(T value) {
			m_tValue = value;
			return this;
		}
	};

} // Settings

#endif // _UTILITIES_SETTINGS_SETTING_H
