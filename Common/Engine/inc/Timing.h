#pragma once

namespace Timing
{
	void Init();
	int64 GetTimeMS();
	double Seconds(int64&);

	class Delta
	{
	public:
		Delta();

		void Calculate(bool lockFramerate);
		void Reset();

		float Get();
		float GetElapsedTime();
		int GetNumFrames();

	protected:
		DWORD GetCurrTime();

		float m_deltaFrame;
		float m_secsframereal;
		float m_averager;
		float m_currTime;

		DWORD m_timerStart; // time of first initialization by game
		int m_numFrames; // number of frames rendered
	};
}

template <typename T>
class Timer
{
public:
	Timer() { m_timer = 0.0f; }
	Timer(T time) { Set(time); }

	void Elapse(T delta) 
	{
		m_timer -= delta;
		if (m_timer < 0.0f)
			m_timer = 0.0f;
	}

	bool Elapsed() { return (m_timer <= 0.0f); }
	void Set(T time) { m_timer = time; }

protected:
	T m_timer;
};

extern Timing::Delta g_DeltaFrame;