/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include "../SHARED/FILE_PARSER/FileParser.h"
#include <fstream>
#include <sstream>

/**
 * Desktop (Raylib) implementation of FileParser.
 * Reads files from the local filesystem using std::ifstream.
 *
 * Paths are SD-root-relative (e.g. "/LOCATIONS/DESK/MAIN/Desk_Full.json").
 * The "SD" directory in the working directory is treated as the SD root,
 * so a leading '/' is replaced with "SD/".
 */
class RaylibFileParser : public FileParser
{
public:
    std::string load(const std::string& path) override
    {
        std::string fullPath = sdPath(path);
        std::ifstream file(fullPath);
        if (!file.is_open())
            return "";

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

private:
    static std::string sdPath(const std::string& path)
    {
        if (!path.empty() && path[0] == '/')
            return "SD" + path;
        return path;
    }
};
