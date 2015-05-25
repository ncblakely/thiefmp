#pragma once

// Light wrapper around CRITICAL_SECTION objects with optional support for logging locks/unlocks

class CCriticalSection
{
public:
	CCriticalSection()
	{
		InitializeCriticalSection(&m_CS);
		m_name = "unnamed";
	}

	CCriticalSection(const char* name)
	{
		InitializeCriticalSection(&m_CS);
		m_name = name;
	}

	~CCriticalSection()
	{
		DeleteCriticalSection(&m_CS);
	}

	void Lock()
	{
#ifdef LOG_CRIT_SECTIONS
		Logger::PrintFile("CSLog.log", "Entering critical section '%s' (%08x).", m_name, this);
#endif

		EnterCriticalSection(&m_CS);

#ifdef _DEBUG
		assert(m_CS.LockCount <= 0);
#endif
	}

	void Unlock()
	{
#ifdef LOG_CRIT_SECTIONS
		Logger::PrintFile("CSLog.log", "Exiting critical section '%s' (%08x).", m_name, this);
#endif

		LeaveCriticalSection(&m_CS);

#ifdef _DEBUG
		assert(m_CS.LockCount <= 0);
#endif
	}

protected:
	CRITICAL_SECTION m_CS;
	const char* m_name;
}; 