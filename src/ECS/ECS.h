#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>

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
        return nextId++;
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
    std::vector<Entity> GetsystemEntities() const;
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
    virtual ~IPool();
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
    std::vector<IPool *> componentPools;

    // vector of component signatures per entity, saying which component is turned "on" for a given entity
    // vector index = entity id
    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, System *> systems;

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
    TSystem *newSystem(new TSystem(std::forward<TArgs>(args)...));
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

    if (componentId >= componentPools.size())
    {
        componentPools.resize(componentId + 1, nullptr);
    }
    if (!componentPools[componentId])
    {
        Pool<TComponent> *newComponentPool = new Pool<TComponent>();
        componentPools[componentId] = newComponentPool;
    }

    Pool<TComponent> *componentPool = Pool<TComponent>(componentPools[componentId]);

    if (entityId >= componentPool->GetSize())
    {
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    return entityComponentSignatures[entityId][componentId];
}

#endif