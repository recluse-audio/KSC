/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include <string>
#include <memory>
#include "Scene.h"

/**
 * Parses a scene JSON string and constructs a fully populated Scene.
 * Used by GameRunner to build the active scene from file content.
 */
class SceneFactory
{
public:
    /**
     * Parse the given JSON string and return a heap-allocated Scene.
     * Ownership is transferred to the caller via unique_ptr.
     */
    std::unique_ptr<Scene> build(const std::string& jsonString);
};
