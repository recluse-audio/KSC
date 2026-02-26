/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include "../SHARED/FILE_PARSER/FileParser.h"
#include <SD.h>
#include <string>

/**
 * ESP32 implementation of FileParser.
 * Reads files from the SD card using the Arduino SD library.
 */
class ESP32FileParser : public FileParser
{
public:
    std::string load(const std::string& path) override
    {
        File file = SD.open(path.c_str());
        if (!file)
            return "";

        std::string content;
        while (file.available())
            content += static_cast<char>(file.read());

        file.close();
        return content;
    }
};
