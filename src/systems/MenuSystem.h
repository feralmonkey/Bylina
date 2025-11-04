#pragma once
#include <entt.hpp>
#include "../systems/ISystem.h"
#include "../components/MenuComponent.h"
#include "../components/TextComponent.h"
#include "../enums/InputState.h"
#include <SDL.h>
#include <spdlog/spdlog.h>
#include <string>

class MenuSystem {
public:

private:

    std::string BuildMenuText(const MenuComponent& menu) {
        std::string message = "//"; // start with newline marker
        for (size_t i = 0; i < menu.options.size(); i++) {
            if (i == menu.currentIndex) {
                message += "> " + menu.options[i] + "//"; // cursor
            }
            else {
                message += "  " + menu.options[i] + "//";
            }
        }
        return message;
    }
};
