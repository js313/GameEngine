#ifndef KEYBOARDMOVEMENTSYSTEM_H
#define KEYBOARDMOVEMENTSYSTEM_H

#include <SDL2/SDL.h>
#include <string>

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Logger/Logger.h"

class KeyboardMovementSystem : public System
{
public:
    KeyboardMovementSystem()
    {
        RequireComponent<KeyboardControlledComponent>();
        RequireComponent<RigidBodyComponent>();
        RequireComponent<SpriteComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->Subscribe<KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent &event)
    {
        for (const auto &entity : GetSystemEntities())
        {
            const auto &keyboardControlledComponent = entity.GetComponent<KeyboardControlledComponent>();
            auto &spriteComponent = entity.GetComponent<SpriteComponent>();
            auto &rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();

            switch (event.keyPressed)
            {
            case SDLK_UP:
                rigidBodyComponent.velocity = keyboardControlledComponent.upVelocity;
                spriteComponent.srcRect.y = spriteComponent.srcRect.h * 0;
                break;
            case SDLK_RIGHT:
                rigidBodyComponent.velocity = keyboardControlledComponent.rightVelocity;
                spriteComponent.srcRect.y = spriteComponent.srcRect.h * 1;
                break;
            case SDLK_DOWN:
                rigidBodyComponent.velocity = keyboardControlledComponent.downVelocity;
                spriteComponent.srcRect.y = spriteComponent.srcRect.h * 2;
                break;
            case SDLK_LEFT:
                rigidBodyComponent.velocity = keyboardControlledComponent.leftVelocity;
                spriteComponent.srcRect.y = spriteComponent.srcRect.h * 3;
                break;

            default:
                break;
            }
        }
    }
};

#endif