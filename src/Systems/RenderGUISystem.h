#ifndef RENDER_GUI_SYSTEM_H
#define RENDER_GUI_SYSTEM_H

#include <numbers>

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

            static float enemyXScale = 1.0f, enemyYScale = 1.0f, enemyRotation = 0;
            ImGui::InputFloat("Enemy X Scale", &enemyXScale);
            ImGui::InputFloat("Enemy Y Scale", &enemyYScale);
            ImGui::InputFloat("Enemy Rotation", &enemyRotation);

            static float enemyXVelocity = 0.0f, enemyYVelocity = 0.0f;
            ImGui::InputFloat("Enemy X Velocity", &enemyXVelocity);
            ImGui::InputFloat("Enemy Y Velocity", &enemyYVelocity);

            static float enemyProjectileAngle = 0.0, enemyProjectileSpeed = 100.0;
            ImGui::InputFloat("Enemy Projectile Speed", &enemyProjectileSpeed);
            ImGui::InputFloat("Enemy Projectile Angle", &enemyProjectileAngle);
            glm::vec2 enemyProjectileVelocity = glm::vec2(enemyProjectileSpeed * cos(enemyProjectileAngle * (std::numbers::pi / 180.0)), enemyProjectileSpeed * sin(enemyProjectileAngle * (std::numbers::pi / 180.0)));
            static int enemyProjectileRepeatFrequency = 5000, enemyProjectileDuration = 1000;
            ImGui::InputInt("Enemy Projectile Frequency", &enemyProjectileRepeatFrequency);
            ImGui::InputInt("Enemy Projectile Duration", &enemyProjectileDuration);

            static int enemyHealthpercentage = 100;
            ImGui::InputInt("Enemy Health Percentage", &enemyHealthpercentage);

            static const char *items[] = {"Tank", "Truck"};
            static const char *sprites[] = {"tank-image", "truck-image"};
            static int item_selected = 0;
            ImGui::ListBox("Enemy Type", &item_selected, items, IM_ARRAYSIZE(items), 4);

            if (ImGui::Button("Create new enemy"))
            {
                Entity newEnemy = registry->CreateEntity();
                newEnemy.Group("enemies");
                newEnemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(enemyXScale, enemyYScale), enemyRotation);
                newEnemy.AddComponent<RigidBodyComponent>(glm::vec2(enemyXVelocity, enemyYVelocity));
                newEnemy.AddComponent<SpriteComponent>(sprites[item_selected], 1, 32, 32);
                newEnemy.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0.0, 0.0));
                newEnemy.AddComponent<ProjectileEmitterComponent>(enemyProjectileVelocity, enemyProjectileRepeatFrequency, enemyProjectileDuration, 10, false);
                newEnemy.AddComponent<HealthComponent>(enemyHealthpercentage);
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