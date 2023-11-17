#ifndef __SOUND_PLAYER_H__
#define __SOUND_PLAYER_H__

#include <dshow.h>

#pragma comment(lib, "strmiids.lib")

static class SoundPlayer
{
private:
	static IGraphBuilder* graphBuilder;
	static IMediaControl* mediaControl;
	static IMediaEvent* mediaEvent;

public:
	static bool Initialize();
	static void Release();

	static void Play(const wchar_t* path);
	static void Stop();
};

#endif