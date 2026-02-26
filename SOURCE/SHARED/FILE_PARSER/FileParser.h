/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include <string>

/**
 * Abstract base class for reading a file from storage and returning its
 * contents as a string. Subclass per platform (ESP32, Raylib desktop, etc.)
 * to provide the appropriate file I/O implementation.
 */
class FileParser
{
public:
    virtual ~FileParser() = default;

    /**
     * Read the file at the given path and return its full contents as a string.
     * Returns an empty string if the file cannot be opened.
     */
    virtual std::string load(const std::string& path) = 0;

    /**
     * Write content to the file at the given path, overwriting any existing data.
     */
    virtual void writeToFile(const std::string& path, const std::string& content) = 0;
};
