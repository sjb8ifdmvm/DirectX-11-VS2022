#include "Timer.h"

Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

double Timer::GetMilisecondsElapsed()
{
	if (IsRunning)
	{
		std::chrono::duration elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else
	{
		std::chrono::duration elapsed = std::chrono::duration<double, std::milli>(stop - start);
		return elapsed.count();
	}
}

void Timer::Restart()
{
	IsRunning = true;
	start = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!IsRunning)
		return false;
	else
	{
		stop = std::chrono::high_resolution_clock::now();
		IsRunning = false;
		return true;
	}
}

bool Timer::Start()
{
	if (IsRunning)
	{
		return false;
	}
	else
	{
		start = std::chrono::high_resolution_clock::now();
		IsRunning = true;
		return true;
	}
}