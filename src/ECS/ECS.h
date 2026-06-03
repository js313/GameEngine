#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>

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

template <typename T> // Makes it so that Component<Movement> is a totally different class as Component<Collider>
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
    int GetId() const;
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

class Registry
{
};

#endif