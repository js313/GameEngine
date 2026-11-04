#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Logger/Logger.h"

class MovementSystem : public System
{
public:
    MovementSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->Subscribe<CollisionEvent>(this, &MovementSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event)
    {
        Logger::Log("DamageSystem received CollisionEvent between Entity " + std::to_string(event.a.GetId()) + " and Entity " + std::to_string(event.b.GetId()));
        Entity a = event.a;
        Entity b = event.b;

        if ((a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")))
        {
            OnEnemyHitObstacle(b, a);
        }
        else if ((b.BelongsToGroup("obstacles") && a.BelongsToGroup("enemies")))
        {
            OnEnemyHitObstacle(a, b);
        }
    }

    void OnEnemyHitObstacle(Entity enemy, Entity obstacle)
    {
        if (!enemy.HasComponent<RigidBodyComponent>())
            return;
        RigidBodyComponent &rigidBodyComponent = enemy.GetComponent<RigidBodyComponent>();

        if (enemy.HasComponent<SpriteComponent>())
        {
            SpriteComponent &spriteComponent = enemy.GetComponent<SpriteComponent>();
            if (rigidBodyComponent.velocity.x != 0)
            {
                spriteComponent.flip = spriteComponent.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
            if (rigidBodyComponent.velocity.y != 0)
            {
                spriteComponent.flip = spriteComponent.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
            }
        }

        rigidBodyComponent.velocity.x *= -1;
        rigidBodyComponent.velocity.y *= -1;
    }

    void Update(double deltaTime)
    {
        for (Entity entity : GetSystemEntities())
        {
            TransformComponent &transform = entity.GetComponent<TransformComponent>();
            const RigidBodyComponent &rigidBody = entity.GetComponent<RigidBodyComponent>();

            transform.position.x += rigidBody.velocity.x * deltaTime;
            transform.position.y += rigidBody.velocity.y * deltaTime;

            bool isOutOfBounds = transform.position.x < 0 || transform.position.y < 0 || transform.position.x > Game::mapWidth || transform.position.y > Game::mapHeight;
            if (isOutOfBounds && !entity.HasTag("player"))
            {
                entity.Kill();
            }
        }
    }
};

#endif