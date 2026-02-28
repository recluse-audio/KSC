#pragma once
#include "FILE_OPERATOR/FileOperator.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

/**
 * In-test FileOperator. Serves named files from an in-memory map and writes
 * real files to disk. listDirectory("/GAME_STATE") resolves from the in-memory
 * map; all other paths scan the real filesystem.
 *
 * diskRoot (optional): when set, virtual paths (leading '/') that are not in
 * the in-memory map fall back to diskRoot + path on disk. Real paths (no
 * leading '/') always fall back to disk directly.
 */
class TestFileOperator : public FileOperator
{
public:
    std::map<std::string, std::string> files;
    std::string diskRoot; // e.g. "KSC_DATA"

    std::string load(const std::string& path) override
    {
        auto it = files.find(path);
        if (it != files.end()) return it->second;

        std::string diskPath = (!diskRoot.empty() && !path.empty() && path[0] == '/')
                             ? diskRoot + path
                             : path;
        std::ifstream f(diskPath);
        if (!f.is_open()) return "";
        std::ostringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    void writeToFile(const std::string& path, const std::string& content) override
    {
        fs::path p(path);
        fs::create_directories(p.parent_path());
        std::ofstream f(p);
        f << content;
    }

    void appendToFile(const std::string&, const std::string&) override {}

    std::vector<std::string> listDirectory(const std::string& dirPath) override
    {
        // Resolve in-memory paths by prefix
        if (!dirPath.empty() && dirPath[0] == '/')
        {
            std::vector<std::string> names;
            std::string prefix = dirPath + "/";
            for (auto& [path, _] : files)
                if (path.rfind(prefix, 0) == 0)
                    names.push_back(path.substr(prefix.size()));
            return names;
        }

        // Real filesystem directory
        std::vector<std::string> entries;
        if (!fs::exists(dirPath)) return entries;
        for (auto& entry : fs::directory_iterator(dirPath))
            entries.push_back(entry.path().filename().string());
        return entries;
    }
};
