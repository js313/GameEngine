#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>
#include <memory>

#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

// We use a bitset (1s and 0s) to keep track of which components an entity has
// Also helps keep track of which entities a system is interested in
typedef std::bitset<MAX_COMPONENTS> Signature; // non-type template parameter

struct IComponent
{
protected:
    // Ids used to group same components regardless of instantiation/object creation
    static int nextId; // global counter for component types
};

template <typename T> // Makes it so that Component<Movement> is a totally different class than Component<Collider>
class Component : public IComponent
{
public:
    static int GetId() // static method so that a seperate object of the same class e.g. Component<Movement> gets the same id
    {
        static int id = nextId++;
        return id;
    }
};

class Entity
{
private:
    int id;

public:
    Entity(int id) : id(id) {};
    Entity(const Entity &entity) = default;
    int GetId() const;

    Entity &operator=(const Entity &other) = default;
    bool operator==(const Entity &other) const { return id == other.GetId(); };
    bool operator!=(const Entity &other) const { return id != other.GetId(); };
    bool operator<(const Entity &other) const { return GetId() < other.GetId(); }

    template <typename TComponent, typename... TArgs>
    void AddComponent(TArgs &&...args);
    template <typename TComponent>
    void RemoveComponent();
    template <typename TComponent>
    bool HasComponent() const;
    template <typename TComponent>
    TComponent &GetComponent() const;

    class Registry *registry; // forward declaration, as Registry class is defined later
};

class System
{
private:
    Signature componentSignature; // components this system affects/handles(represented by bits)
    std::vector<Entity> entities;

public:
    System() = default;
    ~System() = default;
    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature &GetComponentSignature() const;

    template <typename TComponent>
    void RequireComponent();
};

// Convention to implement template functions in the .h file itself
template <typename TComponent>
void System::RequireComponent()
{
    const int componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

class IPool
{
public:
    virtual ~IPool() = default;
};

template <typename T>
class Pool : public IPool
{
private:
    std::vector<T> data;

public:
    Pool(int size = 100)
    {
        data.resize(size);
    }
    virtual ~Pool() = default;

    bool isEmpty() const
    {
        return data.empty();
    }

    int GetSize() const
    {
        return data.size();
    }

    void Resize(int n)
    {
        data.resize(n);
    }

    void Clear()
    {
        data.clear();
    }

    void Add(T object)
    {
        data.push_back(object);
    }

    void Set(int index, T object)
    {
        data[index] = object;
    }

    T &Get(int index)
    {
        return static_cast<T &>(data[index]);
    }

    T &operator[](unsigned int index)
    {
        return Get(index);
    }
};

// Manages the creation and destruction of entities/systems/components
class Registry
{
private:
    int numEntities = 0;
    // list of component pools, each pool contains all the data for a certain component type
    // vector index = component type ID
    // pool index = entity ID
    std::vector<std::shared_ptr<IPool>> componentPools;

    // vector of component signatures per entity, saying which component is turned "on" for a given entity
    // vector index = entity id
    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;

public:
    Registry() = default;

    Entity CreateEntity();

    void AddEntityToSystem(Entity entity);
    void AddEntityToSystems(Entity entity);

    template <typename TComponent, typename... TArgs>
    void AddComponent(Entity entity, TArgs &&...args);

    template <typename TComponent>
    void RemoveComponent(Entity entity);

    template <typename TComponent>
    bool HasComponent(Entity entity) const;

    template <typename TComponent>
    TComponent &GetComponent(Entity entity) const;

    template <typename TSystem, typename... TArgs>
    void AddSystem(TArgs &&...args);

    template <typename TSystem>
    void RemoveSystem();

    template <typename TSystem>
    bool HasSystem() const;

    template <typename TSystem>
    TSystem &GetSystem() const;

    void Update();
};

template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs &&...args)
{
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems[std::type_index(typeid(TSystem))] = newSystem;
}

template <typename TSystem>
void Registry::RemoveSystem()
{
    systems.erase(std::type_index(typeid(TSystem)));
}

template <typename TSystem>
bool Registry::HasSystem() const
{
    return systems.count(std::type_index(typeid(TSystem)));
}

template <typename TSystem>
TSystem &Registry::GetSystem() const
{
    return *static_pointer_cast<TSystem>(systems.at(typeid(TSystem)));
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs &&...args)
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    if (componentId >= (int)componentPools.size())
    {
        componentPools.resize(componentId + 1, nullptr);
    }
    if (!componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    if (entityId >= componentPool->GetSize())
    {
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Added component with id = " + std::to_string(componentId) + " to entity with id = " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Removed component with id = " + std::to_string(componentId) + " from entity with id = " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    return entityComponentSignatures[entityId][componentId];
}

template <typename TComponent>
TComponent &Registry::GetComponent(Entity entity) const
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}

template <typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs &&...args)
{
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent &Entity::GetComponent() const
{
    return registry->GetComponent<TComponent>(*this);
}

#endif