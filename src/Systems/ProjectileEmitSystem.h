#ifndef PROJECTILEEMITSYSTEM_H
#define PROJECTILEEMITSYSTEM_H

#include <memory.h>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/KeyboardControlledComponent.h"

class ProjectileEmitSystem : public System
{
    void SpawnProjectile(const TransformComponent &transform, glm::vec2 projectileVelocity, Entity &entity, Registry *registry, ProjectileEmitterComponent &projectileEmitter)
    {
        glm::vec2 projectilePosition = transform.position;
        if (entity.HasComponent<SpriteComponent>())
        {
            const SpriteComponent &sprite = entity.GetComponent<SpriteComponent>();
            projectilePosition.x += (transform.scale.x * sprite.width / 2);
            projectilePosition.y += (transform.scale.y * sprite.height / 2);
        }
        Entity projectile = registry->CreateEntity();
        projectile.AddComponent<TransformComponent>(projectilePosition);
        projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
        projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
        projectile.AddComponent<BoxColliderComponent>(4, 4);
        projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
        projectileEmitter.lastEmissionTime = SDL_GetTicks();
    }

public:
    ProjectileEmitSystem()
    {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(std::unique_ptr<Registry> &registry)
    {
        for (Entity entity : GetSystemEntities())
        {
            ProjectileEmitterComponent &projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
            const TransformComponent &transform = entity.GetComponent<TransformComponent>();
            bool isKeyboardControlled = entity.HasComponent<KeyboardControlledComponent>();

            if (!isKeyboardControlled && (int)SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency)
            {
                SpawnProjectile(transform, projectileEmitter.projectileVelocity, entity, registry.get(), projectileEmitter);
            }
        }
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus, std::unique_ptr<Registry> &registry)
    {
        eventBus->Subscribe<KeyPressedEvent>(this, &ProjectileEmitSystem::OnFirePressed);
    }

    void OnFirePressed(KeyPressedEvent &event)
    {
        for (Entity &entity : GetSystemEntities())
        {
            ProjectileEmitterComponent &projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
            bool isKeyboardControlled = entity.HasComponent<KeyboardControlledComponent>();
            bool hasVelocity = entity.HasComponent<RigidBodyComponent>();

            if (isKeyboardControlled && hasVelocity && event.keyPressed == SDLK_SPACE && (int)SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency)
            {
                const TransformComponent &transform = entity.GetComponent<TransformComponent>();
                const RigidBodyComponent &rigidBody = entity.GetComponent<RigidBodyComponent>();
                glm::vec2 projectileVelocity = rigidBody.velocity != glm::vec2(0.0f, 0.0f) ? glm::normalize(rigidBody.velocity) * projectileEmitter.projectileVelocity.x : projectileEmitter.projectileVelocity;
                SpawnProjectile(transform, projectileVelocity, entity, entity.registry, projectileEmitter);
            }
        }
    }
};

#endif