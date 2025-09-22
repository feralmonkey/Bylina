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
    //void Update(entt::registry& registry) {
    //    auto view = registry.view<MenuComponent>();
    //    for (auto entity : view) {
    //        auto& menu = view.get<MenuComponent>(entity);
    //        UpdateMenu(registry, entity, menu);
    //    }
    //}

private:
    //void UpdateMenu(entt::registry& registry, entt::entity entity, MenuComponent& menu) {
    //    if (!menu.isActive) return;

    //    // Update TextComponent to reflect cursor position
    //    if (!registry.all_of<TextComponent>(entity)) {
    //        registry.emplace<TextComponent>(entity, BuildMenuText(menu));
    //    }
    //    else {
    //        auto& textComp = registry.get<TextComponent>(entity);
    //        textComp.text = BuildMenuText(menu);
    //    }
    //}

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
