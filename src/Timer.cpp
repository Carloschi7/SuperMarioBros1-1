#include "Timer.h"

Timer::Timer()
	:m_Tp_Begin(steady_clock::now())
{
}

bool Timer::HavePassedAtLeast(float fSeconds) const
{
	time_point tp = steady_clock::now();
	duration dur(tp - m_Tp_Begin);
	return dur.count() >= fSeconds;
}

float Timer::ElapsedTime() const
{
	time_point tp = steady_clock::now();
	duration dur(tp - m_Tp_Begin);
	return dur.count();
}

float Timer::TimeElapsedFrom(const time_point& tp) const
{
	duration dur(tp - m_Tp_Begin);
	return dur.count();
}
