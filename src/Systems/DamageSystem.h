#ifndef DAMAGE_SYSTEM_H
#define DAMAGE_SYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Logger/Logger.h"

class DamageSystem : public System
{
    void OnPlayerProjectileCollision(Entity player, Entity projectile)
    {
        ProjectileComponent projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (!projectileComponent.isFriendly)
        {
            HealthComponent &playerHealthComponent = player.GetComponent<HealthComponent>();
            playerHealthComponent.healthPercent -= projectileComponent.hitPercentage;

            if (playerHealthComponent.healthPercent <= 0)
            {
                player.Kill();
            }

            projectile.Kill();
        }
    }

    void OnEnemyProjectileCollision(Entity enemy, Entity projectile)
    {
        ProjectileComponent projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (projectileComponent.isFriendly)
        {
            HealthComponent &enemyHealthComponent = enemy.GetComponent<HealthComponent>();
            enemyHealthComponent.healthPercent -= projectileComponent.hitPercentage;

            if (enemyHealthComponent.healthPercent <= 0)
            {
                enemy.Kill();
            }

            projectile.Kill();
        }
    }

public:
    DamageSystem()
    {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->Subscribe<CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event)
    {
        Logger::Log("DamageSystem received CollisionEvent between Entity " + std::to_string(event.a.GetId()) + " and Entity " + std::to_string(event.b.GetId()));
        // event.a.Kill();
        // event.b.Kill();
        if (event.a.HasTag("player") && event.b.BelongsToGroup("projectiles"))
            OnPlayerProjectileCollision(event.a, event.b);
        if (event.b.HasTag("player") && event.a.BelongsToGroup("projectiles"))
            OnPlayerProjectileCollision(event.b, event.a);

        if (event.a.BelongsToGroup("enemies") && event.b.BelongsToGroup("projectiles"))
            OnEnemyProjectileCollision(event.a, event.b);
        if (event.b.BelongsToGroup("enemies") && event.a.BelongsToGroup("projectiles"))
            OnEnemyProjectileCollision(event.b, event.a);
    }

    void Update()
    {
    }
};

#endif