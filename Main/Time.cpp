#include <Windows.h>
#include "Time.h"

__int64 Time::beforeTime = 0;
__int64 Time::afterTime = 0;
__int64 Time::periodFrequency = 0;
double Time::timeScale = 0.0;
double Time::deltaTime = 0.0;

void Time::Initialize()
{
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&periodFrequency));
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&beforeTime));

	timeScale = 1.0 / static_cast<double>(periodFrequency);
}

void Time::Update()
{
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&afterTime));

	deltaTime = (afterTime - beforeTime) * timeScale;
	beforeTime = afterTime;
}

double Time::GetDeltaTime()
{
	return deltaTime;
}