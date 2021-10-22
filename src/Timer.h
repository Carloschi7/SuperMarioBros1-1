#pragma once
#include <chrono>

class Timer
{
public:
	using steady_clock = std::chrono::steady_clock;
	using duration = std::chrono::duration<float>;
	using time_point = std::chrono::steady_clock::time_point;
public:
	Timer();
	Timer(const Timer&) = default;
	//Time elapsed from object construction/starting point update
	bool HavePassedAtLeast(float fSeconds) const;
	float ElapsedTime() const;
	float TimeElapsedFrom(const time_point& tp) const;
	inline void UpdateStartingTimePoint(const time_point& tp) { m_Tp_Begin = tp; }
	inline void SetCurrentTimePointAsBegin() { m_Tp_Begin = steady_clock::now(); }
public:
	//Utility variable needed if the timer has different implementation
	//in a class instance
	uint8_t state = 0;
private:
	time_point m_Tp_Begin;
};