#ifndef RENDER_GUI_SYSTEM_H
#define RENDER_GUI_SYSTEM_H

#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

#include "../ECS/ECS.h"

class RenderGUISystem : public System
{
public:
    RenderGUISystem() = default;

    void Update(const std::unique_ptr<Registry> &registry)
    {
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn enemies"))
        {
            static int enemyXPos = 0, enemyYPos = 0;
            ImGui::InputInt("Enemy X Position", &enemyXPos);
            ImGui::InputInt("Enemy Y Position", &enemyYPos);

            if (ImGui::Button("Create new enemy"))
            {
                Entity newEnemy = registry->CreateEntity();
                newEnemy.Group("enemies");
                newEnemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(1.0, 1.0), 0.0);
                newEnemy.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
                newEnemy.AddComponent<SpriteComponent>("tank-image", 1, 32, 32);
                newEnemy.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0.0, 0.0));
                newEnemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0), 5000, 1000, 10, false);
                newEnemy.AddComponent<HealthComponent>(100);
                glm::vec2 healthTextPosition = newEnemy.GetComponent<TransformComponent>().position + glm::vec2(0, -15);
                std::string healthText = std::to_string(newEnemy.GetComponent<HealthComponent>().healthPercent) + "%";
                SDL_Color healthTextColor = {0, 255, 0, 255};
                newEnemy.AddComponent<TextLabelComponent>(healthTextPosition, healthText, "charriot-font-small", healthTextColor, false);
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};

#endif