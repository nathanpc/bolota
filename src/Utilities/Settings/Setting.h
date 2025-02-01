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
#include <tchar.h>

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
		TCHAR* m_key;
		TCHAR* m_description;

	public:
		// Constructors and destructors.
		BaseSetting(const TCHAR* key, const TCHAR* description);
		virtual ~BaseSetting();

		// Getters .
		const TCHAR* Key() const;
		const TCHAR* Description() const;
	};

	/**
	 * Abstraction over a single setting in the configuration manager.
	 */
	template <typename T>
	class Setting : public BaseSetting {
	protected:
		T m_value;

		LSTATUS LoadValue(LPBYTE lpValue, DWORD* lpdwLength);

	public:
		// Constructors and destructors.
		//Setting(const TCHAR* key, const TCHAR* description, T defaultValue);
		Setting(const TCHAR* key, const TCHAR* description,
			DWORD defaultValue) : BaseSetting(key, description) {
			if (Load() != ERROR_SUCCESS) { m_value = defaultValue; }
		};
		Setting(const TCHAR* key, const TCHAR* description,
			TCHAR* defaultValue) : BaseSetting(key, description) {
			if (Load() != ERROR_SUCCESS) { m_value = defaultValue; }
		};

		// Registry management.
		LSTATUS Load();
		bool SaveValue(DWORD dwType, DWORD dwLength, LPCBYTE lpData);
		bool Save(DWORD dwType, DWORD dwLength);

		// Getters and setters.
		T Value() const;
		Setting<T>* SetValue(T value);
	};

	/**
	 * A DWORD-sized setting.
	 */
	class DWordSetting : public Setting<DWORD> {
	public:
		// Constructors and destructors.
		DWordSetting(const TCHAR* key, const TCHAR* description,
			DWORD defaultValue) : Setting<DWORD>(key, description,
				defaultValue) {};

		// Registry management.
		bool Save() { return Setting::Save(REG_DWORD, sizeof(DWORD)); };

		// Getters and setters.
		DWORD Value() const { return m_value; };
		Setting<DWORD>* SetValue(DWORD value) {
			m_value = value;
			return this;
		};
	};

	/**
	 * A simple string setting.
	 */
	class StringSetting : public Setting<TCHAR*> {
	public:
		// Constructors and destructors.
		StringSetting(const TCHAR* key, const TCHAR* description,
			TCHAR* defaultValue) : Setting<TCHAR*>(key, description,
				defaultValue) {};
		virtual ~StringSetting() {
			if (m_value)
				free(m_value);
			m_value = NULL;
		};

		// Registry management.
		bool Save() {
			return Setting::SaveValue(REG_SZ,
				((DWORD)_tcslen(m_value) + 1) * (DWORD)sizeof(TCHAR),
				(LPCBYTE)m_value);
		};

		// Getters and setters.
		TCHAR* Value() const { return m_value; };
		Setting<TCHAR*>* SetValue(TCHAR* value) {
			if (m_value)
				free(m_value);
			m_value = value;
			return this;
		};
	};

} // Settings

#endif // _UTILITIES_SETTINGS_SETTING_H
