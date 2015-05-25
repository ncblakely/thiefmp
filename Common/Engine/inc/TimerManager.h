#pragma once

#include "Timing.h"
#include <vector>

#define MAX_TIMERS 16

enum TimerResult
{
	TR_OK,
	TR_NoUpdate,
};

typedef TimerResult (*_TimerFunc)();

class CTimerManager
{
	struct Timer
	{
		_TimerFunc func;
		int64		callFrequency;
		int64		nextCall;
	};

public:
	CTimerManager();

	bool SetNextCall(uint timerIndex, uint64 time);
	int RegisterTimer(_TimerFunc func, int frequency);	
	void Update();

protected:
	Timer m_TimerList[MAX_TIMERS];
	int m_timerCount;
};

class TimedEvent
{
public:
	TimedEvent(float interval);
	virtual ~TimedEvent() { }

	void Elapse(float delta);

protected:
	virtual void Fire() = 0;

	float m_interval;
	Timer<float> m_timer;
};

class TimerManager
{
public:
	virtual ~TimerManager();

	void AddTimer(TimedEvent* event);
	void Update(float delta);

protected:
	std::vector<TimedEvent*> m_Events;
};