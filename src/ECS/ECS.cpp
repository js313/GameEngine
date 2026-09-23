#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

void Entity::Kill()
{
    registry->KillEntity(*this);
}

void Entity::Tag(const std::string &tag)
{
    registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string &tag) const
{
    return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string &group)
{
    registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string &group) const
{
    return registry->EntityBelongsToGroup(*this, group);
}

void System::AddEntityToSystem(Entity entity)
{
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
    std::erase_if(entities, [&entity](Entity &otherEntity)
                  { return otherEntity == entity; });
}

std::vector<Entity> System::GetSystemEntities() const
{
    return entities;
}

const Signature &System::GetComponentSignature() const
{
    return componentSignature;
}

Entity Registry::CreateEntity()
{
    int entityId = numEntities;

    if (!freeIds.empty())
    {
        entityId = freeIds.front();
        freeIds.pop_front();
    }
    else
    {
        numEntities++;
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    if ((size_t)entityId >= entityComponentSignatures.size())
    {
        entityComponentSignatures.resize(entityId + 1);
    }
    Logger::Log("Entity created with id = " + std::to_string(entityId));

    return entity;
}

void Registry::KillEntity(Entity entity)
{
    entitiesToBeKilled.insert(entity);
}

void Registry::TagEntity(Entity entity, const std::string &tag)
{
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string &tag)
{
    auto it = tagPerEntity.find(entity.GetId());
    if (it != tagPerEntity.end())
    {
        return it->second == tag;
    }
    return false;
}

Entity Registry::GetEntityByTag(const std::string &tag) const
{
    auto it = entityPerTag.find(tag);
    if (it != entityPerTag.end())
    {
        return it->second;
    }
    throw std::runtime_error("No entity found with tag: " + tag);
}

void Registry::RemoveEntityTag(Entity entity)
{
    auto it = tagPerEntity.find(entity.GetId());
    if (it != tagPerEntity.end())
    {
        std::string tag = it->second;
        tagPerEntity.erase(it);
        entityPerTag.erase(tag);
    }
}

void Registry::GroupEntity(Entity entity, const std::string &group)
{
    entitiesPerGroup[group].insert(entity);
    groupPerEntity[entity.GetId()] = group;
}

bool Registry::EntityBelongsToGroup(Entity entity, const std::string &group)
{
    auto it = groupPerEntity.find(entity.GetId());
    if (it != groupPerEntity.end())
    {
        return it->second == group;
    }
    return false;
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string &group) const
{
    std::vector<Entity> entitiesInGroup;
    auto it = entitiesPerGroup.find(group);
    if (it != entitiesPerGroup.end())
    {
        entitiesInGroup.insert(entitiesInGroup.end(), it->second.begin(), it->second.end());
    }
    return entitiesInGroup;
}

void Registry::RemoveEntityGroup(Entity entity)
{
    auto it = groupPerEntity.find(entity.GetId());
    if (it != groupPerEntity.end())
    {
        std::string group = it->second;
        groupPerEntity.erase(it);
        entitiesPerGroup[group].erase(entity);
    }
}

void Registry::AddEntityToSystems(Entity entity)
{
    const int entityId = entity.GetId();

    const Signature &entityComponentSignature = entityComponentSignatures[entityId];

    for (auto &system : systems)
    {
        const Signature &systemComponentSignature = system.second->GetComponentSignature();

        if ((entityComponentSignature & systemComponentSignature) == systemComponentSignature) // only add if entity has the system required components
        {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
    for (auto &system : systems)
    {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::Update()
{
    for (auto entity : entitiesToBeAdded)
    {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    for (auto entity : entitiesToBeKilled)
    {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();

        for (auto &pool : componentPools)
        {
            if (pool)
                pool->RemoveEntityFromPool(entity.GetId());
        }

        freeIds.push_back(entity.GetId());

        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
    }
    entitiesToBeKilled.clear();
}
