#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <memory>

#include "../Logger/Logger.h"
#include "Event.h"

class IEventCallback
{
    virtual void Call(Event &e) = 0;

public:
    virtual ~IEventCallback() = default;
    void Execute(Event &e)
    {
        Call(e);
    }
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
    typedef void (TOwner::*CallbackFunction)(TEvent &);

    TOwner *ownerInstance;
    CallbackFunction callbackFunction;

    virtual void Call(Event &e) override
    {
        std::invoke(callbackFunction, ownerInstance, static_cast<TEvent &>(e));
    }

public:
    EventCallback(TOwner *ownerInstance, CallbackFunction callbackFunction) : ownerInstance(ownerInstance), callbackFunction(callbackFunction) {}
    virtual ~EventCallback() override = default;
};

typedef std::vector<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus
{
private:
    std::unordered_map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
    EventBus()
    {
        Logger::Log("EventBus created");
    }

    ~EventBus()
    {
        Logger::Log("EventBus destroyed");
    }

    void Reset()
    {
        subscribers.clear();
    }

    template <typename TEvent, typename TOwner>
    void Subscribe(TOwner *ownerInstance, void (TOwner::*callbackFunction)(TEvent &))
    {
        if (!subscribers.contains(typeid(TEvent)))
        {
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        std::unique_ptr<EventCallback<TOwner, TEvent>> callback = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        subscribers[typeid(TEvent)]->push_back(std::move(callback));
    }

    template <typename TEvent, typename... TArgs>
    void Publish(TArgs &&...args)
    {
        TEvent event(std::forward<TArgs>(args)...);
        if (subscribers.contains(typeid(TEvent)))
        {
            for (const auto &callback : *subscribers[typeid(TEvent)])
            {
                callback->Execute(event);
            }
        }
    }
};

#endif