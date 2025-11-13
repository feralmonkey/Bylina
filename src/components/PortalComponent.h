#pragma once
#include <string>
#include <glm/glm.hpp>

struct PortalComponent {
    std::string targetMap;   // "overworld", "init", etc.
    glm::vec2 spawnPosition; // where to drop the player in the new map (world coords)
};
