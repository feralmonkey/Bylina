#pragma once

#include "../ecs/ecs.h"
#include <spdlog/spdlog.h> 
#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_sdl.h>
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/HealthComponent.h"


class RenderGUISystem : public System {
	public:
		RenderGUISystem() = default;

		void Update(const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore) {
			ImGui::NewFrame();

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;
			if (ImGui::Begin("Spawn Enemies", NULL, windowFlags)) {
				static int enemyXPos = 0;
				static int enemyYPos = 0;
				static int enemyXVelocity = 0;
				static int enemyYVelocity = 0;

				// drop down to select texture
				static int projectileVelocity = 0;
				static int projectileAngle = 0;
				static int projectileFrequency = 3000;
				static int projectileLifetime = 5000;
				static int healthValue = 100;

				// enemy pos
				ImGui::InputInt("Enemy X Position", &enemyXPos);
				ImGui::InputInt("Enemy Y Position", &enemyYPos);

				// enemy velocity
				ImGui::InputInt("Enemy X Velocity", &enemyXVelocity);
				ImGui::InputInt("Enemy Y Velocity", &enemyYVelocity);

				// texture selector combo box
				static int currentSelectedComboBoxId = 0;

				std::vector<std::string> textures;
				for (auto entity : GetSystemEntities()) {
					if (entity.BelongsToGroup("enemies")) {
						auto component = entity.GetComponent<SpriteComponent>();
						textures.push_back(component.assetId);
					}
				}
				std::vector<const char*> cstr_textures;
				for (auto& s : textures) {
					cstr_textures.push_back(s.c_str());
				}

				const char* combo_label = cstr_textures[currentSelectedComboBoxId];
				if (ImGui::BeginCombo("Texture Select", combo_label)) {
					for (int n = 0; n < cstr_textures.size(); n++)
					{
						const bool is_selected = (currentSelectedComboBoxId == n);
						if (ImGui::Selectable(cstr_textures[n], is_selected))
							currentSelectedComboBoxId = n;

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}


				// projectile values
				ImGui::InputInt("Projectile Velocity", &projectileVelocity);
				ImGui::InputInt("Projectile Angle", &projectileAngle);
				ImGui::InputInt("Projectile Frequency", &projectileFrequency);
				ImGui::InputInt("Projectile Lifetime", &projectileLifetime);

				// health
				ImGui::InputInt("Enemy Health", &healthValue);
				
				// a button to create a new entity
				if (ImGui::Button("Spawn", ImVec2(0, 0))) {
					Entity enemy = registry->CreateEntity();
					enemy.Group("enemies");
					enemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(1.0, 1.0), 0.0);
					enemy.AddComponent<RigidBodyComponent>(glm::vec2(enemyXVelocity, enemyYVelocity));
					enemy.AddComponent<SpriteComponent>(combo_label, 32, 32, 1);
					enemy.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
					enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), projectileLifetime, projectileFrequency, 10, false);
					enemy.AddComponent<HealthComponent>(healthValue);
				}
			}
			ImGui::End();

			ImGui::Render();
			ImGuiSDL::Render(ImGui::GetDrawData());
		}
};