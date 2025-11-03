#pragma once

#include <format>
#include <string>
#include <unordered_map>
#include <memory>

namespace utils {

	class DebugLog {
	public:
		virtual ~DebugLog() = default;

		virtual void print(const std::string& _str) = 0;

		virtual void println(const std::string& _str) = 0;
	};

	using DebugLogPtr = std::shared_ptr<DebugLog>;

	class DebugProvider {
	public:
		static DebugLogPtr openDebugLog(const std::string& _key, const std::string& _filePath);

		// static DebugLogPtr getDebugLog(const std::string& _key);

	private:
		static std::unordered_map<std::string, DebugLogPtr> debugLogMap;
	};

}