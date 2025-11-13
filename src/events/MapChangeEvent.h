#pragma once
#include <string>
#include <glm/glm.hpp>

struct MapChangeEvent {
    std::string targetMap;
    glm::vec2 spawnPosition;
};