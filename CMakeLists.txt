cmake_minimum_required(VERSION 3.16)
project(Bylina LANGUAGES CXX)

# ==== Compiler / Language Settings ====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

# ==== Build Types ====
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ==== Source Files ====
set(SRC_FILES
    src/main.cpp
    src/Game.cpp
    src/MapLoader.cpp
    src/assetstore/AssetStore.cpp
)

set(HEADER_FILES
    src/Game.h
    src/MapLoader.h
    src/assetstore/AssetStore.h
    src/components/AnimationComponent.h
    src/components/BoxColliderComponent.h
    src/components/CameraFollowComponent.h
    src/components/HealthComponent.h
    src/components/KeyboardControlComponent.h
    src/components/PlayerComponent.h
    src/components/RigidBodyComponent.h
    src/components/ScriptComponent.h
    src/components/SpriteComponent.h
    src/components/TextComponent.h
    src/components/TransformComponent.h
    src/components/TagComponents.h
    src/events/CollisionEvent.h
    src/events/KeyPressedEvent.h
    src/systems/AnimationSystem.h
    src/systems/CameraMovementSystem.h
    src/systems/CollisionSystem.h
    src/systems/KeyboardControlSystem.h
    src/systems/MovementSystem.h
    src/systems/RenderColliderSystem.h
    src/systems/RenderGUISystem.h
    src/systems/RenderSystem.h
    src/systems/RenderTextSystem.h
    src/systems/ScriptSystem.h
    libs/nlohmann/json.hpp
)

# ==== Include Directories ====
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/libs
    ${PROJECT_SOURCE_DIR}/libs/spdlog
    ${PROJECT_SOURCE_DIR}/libs/entt
    ${PROJECT_SOURCE_DIR}/libs/sol
    ${PROJECT_SOURCE_DIR}/libs/sdl2/include
    /usr/include/lua5.3       # <-- add this line	
)

# ==== Find SDL2 Packages ====
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# ==== Handle Lua Cross-Platform ====
if(WIN32)
    message(STATUS "Configuring Lua for Windows build")
    # Assuming your Windows lua headers and libs are in libs/lua
    include_directories(${PROJECT_SOURCE_DIR}/libs/lua)
    set(LUA_LIBRARIES ${PROJECT_SOURCE_DIR}/libs/lua/lua53.lib)
else()
    message(STATUS "Configuring Lua for Linux build")
    # On Ubuntu, use system Lua (liblua5.3-dev)
    set(LUA_INCLUDE_DIR /usr/include/lua5.3)
    set(LUA_LIBRARIES /usr/lib/x86_64-linux-gnu/liblua5.3.so)
    include_directories(${LUA_INCLUDE_DIR})
endif()

# ==== Executable ====
add_executable(Bylina ${SRC_FILES} ${HEADER_FILES})

add_custom_target(sync_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
        COMMENT "Syncing assets (including Lua scripts) before run..."
)

add_dependencies(Bylina sync_assets)

# ==== Link Libraries ====
target_link_libraries(Bylina
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    ${LUA_LIBRARIES}
)

# ==== Preprocessor Defines ====
target_compile_definitions(Bylina PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _CONSOLE
)

# ==== Assets Copy ====
add_custom_command(TARGET Bylina POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Bylina>/assets
)