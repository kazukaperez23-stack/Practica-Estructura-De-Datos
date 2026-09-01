#pragma once
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace ds {

inline std::vector<std::string> loadWords(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Could not open dataset file: " + path);
    }
    std::vector<std::string> words;
    words.reserve(120000);
    std::string line;
    while (std::getline(in, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (!line.empty()) words.push_back(line);
    }
    return words;
}

inline bool isSorted(const std::vector<std::string>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i - 1] > v[i]) return false;
    }
    return true;
}

inline std::string formatBytes(double bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int u = 0;
    while (bytes >= 1024.0 && u < 3) {
        bytes /= 1024.0;
        ++u;
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << bytes << " " << units[u];
    return oss.str();
}

} // namespace ds
