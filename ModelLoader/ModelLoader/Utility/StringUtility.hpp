#pragma once

#include <locale.h>
#include <string>
#include <variant>
#include <cstring>

namespace su {

	template <class String>
	String getExtension(const String& _path) {
		auto findIdx = _path.rfind('.');
		auto extension = _path.substr(findIdx + 1, _path.size() - 1);
		return extension;
	}

	inline std::string convertToString(const std::wstring& _wstr) {
		setlocale(LC_ALL, "Japanese");
		char cBuffer[1024];

		size_t retValue;
		wcstombs_s(&retValue, cBuffer, 1024, _wstr.data(), 1024 - 1);

		return { cBuffer };
	}

	inline std::wstring convertToWString(const std::string& _str) {
		setlocale(LC_ALL, "Japanese");
		size_t len = _str.size();  // •¶š—ñ‚Ì’·‚³
		size_t convLen;            // •ÏŠ·Œã•¶š—ñ

		// •¶š—ñ‚ÌŠi”[êŠ
		wchar_t* wc = new wchar_t[sizeof(wchar_t) * (len + 2)];
		mbstowcs_s(&convLen, wc, len + 1, _str.c_str(), _TRUNCATE);

		std::wstring res{ wc };
		delete[] wc;

		return res;
	};

	inline std::wstring getWString(const std::variant<std::string, std::wstring>& _variant) {
		if (std::holds_alternative<std::string>(_variant)) {
			return convertToWString(std::get<std::string>(_variant));
		}

		return std::get<std::wstring>(_variant);
	}

	inline std::wstring rep(std::wstring& _wstr, const std::wstring& _target, const std::wstring& _rep) {
		if (!_target.empty()) {
			std::wstring::size_type pos{ 0 };
			while ((pos = _wstr.find(_target, pos)) != std::wstring::npos) {
				_wstr.replace(pos, _target.length(), _rep);
				pos += _rep.length();
			}
		}

		return _wstr;
	}

}