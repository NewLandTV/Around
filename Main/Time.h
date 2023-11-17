#ifndef __TIME_H__
#define __TIME_H__

static class Time
{
private:
	static __int64 beforeTime;
	static __int64 afterTime;
	static __int64 periodFrequency;
	static double timeScale;
	static double deltaTime;

public:
	static void Initialize();
	static void Update();

	static double GetDeltaTime();
};

#endif