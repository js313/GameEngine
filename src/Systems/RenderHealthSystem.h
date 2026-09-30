#ifndef RENDERHEALTHSYSTEM_H
#define RENDERHEALTHSYSTEM_H

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../AssetStore/AssetStore.h"

class RenderHealthSystem : public System
{
public:
    RenderHealthSystem()
    {
        RequireComponent<HealthComponent>();
        RequireComponent<TransformComponent>();
        RequireComponent<TextLabelComponent>();
    }

    void Update(std::unique_ptr<AssetStore> &assetStore, SDL_Renderer *renderer, const SDL_Rect &camera)
    {
        for (auto entity : GetSystemEntities())
        {
            const auto &healthComponent = entity.GetComponent<HealthComponent>();
            const auto &transformComponent = entity.GetComponent<TransformComponent>();
            auto &textLabelComponent = entity.GetComponent<TextLabelComponent>();

            glm::vec2 chopperHealthTextPosition = transformComponent.position + glm::vec2(0, -15);
            std::string chopperHealthText = std::to_string(healthComponent.healthPercent) + "%";
            SDL_Color chopperHealthTextColor = healthComponent.healthPercent > 80 ? SDL_Color{0, 255, 0, 255} : (healthComponent.healthPercent > 50 ? SDL_Color{255, 255, 0, 255} : SDL_Color{255, 0, 0, 255});

            textLabelComponent.position = chopperHealthTextPosition;
            textLabelComponent.text = chopperHealthText;
            textLabelComponent.color = chopperHealthTextColor;

            int barWidth = 20; // Width of the health bar
            int barHeight = 3; // Height of the health bar
            int filledWidth = (healthComponent.healthPercent * barWidth) / 100;

            SDL_Rect healthBarBackground = {static_cast<int>(transformComponent.position.x) - (camera.x),
                                            static_cast<int>(transformComponent.position.y) - (camera.y) - 5,
                                            barWidth,
                                            barHeight};

            SDL_Rect healthBarFilled = {healthBarBackground.x,
                                        healthBarBackground.y,
                                        filledWidth,
                                        barHeight};

            SDL_SetRenderDrawColor(renderer, chopperHealthTextColor.r, chopperHealthTextColor.g, chopperHealthTextColor.b, chopperHealthTextColor.a);
            SDL_RenderFillRect(renderer, &healthBarFilled);
        }
    }
};
#endif