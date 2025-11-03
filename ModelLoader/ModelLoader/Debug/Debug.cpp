#include "Debug.hpp"
#include <fstream>

namespace utils {

    class DebugLogImpl : public DebugLog {
    public:
        explicit DebugLogImpl(const std::string& _filePath)
            : ofs{ _filePath }
        {}

        ~DebugLogImpl() {
            if (ofs.is_open()) {
                ofs.close();
            }
        }

        void print(const std::string& _str) override {
            ofs << _str;
        }

        void println(const std::string& _str) override {
            ofs << _str << "\n";
        }

    private:
        std::ofstream ofs;
        
    private:
        DebugLogImpl(const DebugLogImpl&);
        DebugLogImpl& operator=(const DebugLogImpl&);
    };

    std::unordered_map<std::string, DebugLogPtr> DebugProvider::debugLogMap{};

    DebugLogPtr DebugProvider::openDebugLog(const std::string& _key, const std::string& _filePath)
    {
        if (debugLogMap.contains(_key)) return debugLogMap[_key];

        DebugLogPtr debugLogPtr{ new DebugLogImpl{ _filePath } };
        debugLogMap.insert({ _key, debugLogPtr });
        return debugLogMap[_key];
    }

}
