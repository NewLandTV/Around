#include "SoundPlayer.h"

IGraphBuilder* SoundPlayer::graphBuilder;
IMediaControl* SoundPlayer::mediaControl;
IMediaEvent* SoundPlayer::mediaEvent;

bool SoundPlayer::Initialize()
{
	HRESULT result = CoInitialize(nullptr);

	if (FAILED(result))
	{
		return false;
	}

	result = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast<void**>(&graphBuilder));

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundPlayer::Release()
{
	mediaControl->Release();
	mediaEvent->Release();
	graphBuilder->Release();

	CoUninitialize();
}

void SoundPlayer::Play(const wchar_t* path)
{
	HRESULT result = graphBuilder->RenderFile(path, nullptr);

	result = graphBuilder->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&mediaControl));
	result = graphBuilder->QueryInterface(IID_IMediaEvent, reinterpret_cast<void**>(&mediaEvent));

	if (SUCCEEDED(result))
	{
		mediaControl->Run();
	}
}

void SoundPlayer::Stop()
{
	mediaControl->Stop();

	Release();
	Initialize();
}