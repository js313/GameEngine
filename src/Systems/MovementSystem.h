#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

#include "../Logger/Logger.h"

class MovementSystem : public System
{
public:
    MovementSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void Update(double deltaTime)
    {
        for (Entity entity : GetSystemEntities())
        {
            TransformComponent &transform = entity.GetComponent<TransformComponent>();
            const RigidBodyComponent &rigidBody = entity.GetComponent<RigidBodyComponent>();

            transform.position.x += rigidBody.velocity.x * deltaTime;
            transform.position.y += rigidBody.velocity.y * deltaTime;

            Logger::Log("Entity " + std::to_string(entity.GetId()) + " moved to position (" + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y) + ")");
        }
    }
};

#endif