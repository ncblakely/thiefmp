#pragma once

#include <vector>

interface IEventListener
{
	virtual void EventHandler(int eventID, void* eventData) = 0;
};

class EventListener
{
public:
	EventListener(int eventID, IEventListener* listener);
	bool operator == (const EventListener& rhs);

	IEventListener* Listener() const { return m_listener; }
	int EventID() const { return m_eventID; }

protected:
	IEventListener* m_listener;
	int m_eventID;
};

class EventManager
{
public:

	static EventManager* Instance();
	static void Initialize();
	static void Shutdown();

	static void Broadcast(int eventID, void* eventData);
	static bool RegisterListener(int eventID, IEventListener* pListener);
	static bool UnregisterListener(int eventID, IEventListener* pListener);
private:
	typedef std::vector<EventListener> EventList;

	EventList m_listeners;
};