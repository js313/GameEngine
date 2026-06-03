#include "ECS.h"

int Entity::GetId() const
{
    return id;
}

void System::AddEntityToSystem(Entity entity)
{
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
    std::erase_if(entities, [&entity](Entity &otherEntity)
                  { return otherEntity.GetId() == entity.GetId(); });
}

std::vector<Entity> System::GetsystemEntities() const
{
    return entities;
}

const Signature &System::GetComponentSignature() const
{
    return componentSignature;
}