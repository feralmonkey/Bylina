#pragma once
#include <entt.hpp>
#include "../systems/ISystem.h"
#include "../components/MenuComponent.h"
#include "../components/TextComponent.h"
#include "../enums/InputState.h"
#include <SDL.h>
#include <spdlog/spdlog.h>
#include <string>

class MenuSystem : public ISystem {
public:
    void Update(entt::registry& registry) {
        auto view = registry.view<MenuComponent>();
        for (auto entity : view) {
            auto& menu = view.get<MenuComponent>(entity);
            UpdateMenu(registry, entity, menu);
        }
    }

private:
    void UpdateMenu(entt::registry& registry, entt::entity entity, MenuComponent& menu) {
        if (!menu.isActive) return;

        // You probably have some input polling system or SDL events
        // For this example, we'll iterate SDL events each update
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_UP:
                    menu.currentIndex = (menu.currentIndex - 1 + menu.options.size()) % menu.options.size();
                    break;
                case SDL_SCANCODE_DOWN:
                    menu.currentIndex = (menu.currentIndex + 1) % menu.options.size();
                    break;
                case SDL_SCANCODE_X: // Confirm
                    spdlog::info("Selected option: {}", menu.options[menu.currentIndex]);
                    // TODO: trigger action
                    break;
                case SDL_SCANCODE_Z: // Cancel
                    menu.isActive = false;
                    // Remove menu entity and maybe pop InputState
                    if (registry.valid(entity))
                        registry.destroy(entity);
                    break;
                default:
                    break;
                }
            }
        }

        // Update TextComponent to reflect cursor position
        if (!registry.all_of<TextComponent>(entity)) {
            registry.emplace<TextComponent>(entity, BuildMenuText(menu));
        }
        else {
            auto& textComp = registry.get<TextComponent>(entity);
            textComp.text = BuildMenuText(menu);
        }
    }

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
