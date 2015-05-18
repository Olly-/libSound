#include "stdafx.h"
#include "header.h"
#include <windows.h>
#include <iostream>
#include <MMDeviceAPI.h>
#include <stdexcept>

#define DLL_FUNC __declspec(dllexport)

HMODULE module = NULL;
extern HMODULE module;

int GetPluginABIVersion()
{
	return 2;
}

int GetFunctionCount()
{
	return PascalExportCount;
}

int GetFunctionInfo(int Index, void** Address, char** Definition)
{
	if (Index < PascalExportCount)
	{
		*Address = (void*)GetProcAddress(module, PascalExports[Index * 2]);
		strcpy(*Definition, PascalExports[Index * 2 + 1]);
		return Index;
	}
	return -1;
}

int GetTypeCount()
{
	return PascalTypeCount;
}

int GetTypeInfo(int Index, char** Type, char** Definition)
{
	if (Index < PascalTypeCount)
	{
		strcpy(*Type, PascalTypes[Index * 2 + 0]);
		strcpy(*Definition, PascalTypes[Index * 2 + 1]);
		return Index;
	}
	return -1;
}

TSoundReader::TSoundReader(DWORD PID)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	
	CComPtr<IMMDeviceEnumerator> DeviceEnumerator;
	if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&DeviceEnumerator))) {
		throw std::runtime_error("Failed to create device enumerator \n");
	}

	CComPtr<IMMDevice> DefaultDevice;
	if (FAILED(DeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &DefaultDevice))) {
		throw std::runtime_error("Failed to create default device enumerator \n");
	}

	CComPtr<IAudioSessionManager2> pAudioSessionManager2;
	if (FAILED(DefaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (VOID**)&pAudioSessionManager2))) {
		throw std::runtime_error("Failed to audio session manager \n");
	}

	CComPtr<IAudioSessionEnumerator> pAudioSessionEnumerator;
	if (FAILED(pAudioSessionManager2->GetSessionEnumerator(&pAudioSessionEnumerator))) {
		throw std::runtime_error("Failed to get session enumerator \n");
	}

	int SessionCount;
	if (FAILED(pAudioSessionEnumerator->GetCount(&SessionCount))) {
		throw std::runtime_error("Failed to get session enumerator count \n");
	}

	for (INT nSessionIndex = 0; nSessionIndex < SessionCount; nSessionIndex++)
	{
		CComPtr<IAudioSessionControl> pSessionControl;
		if (SUCCEEDED(pAudioSessionEnumerator->GetSession(nSessionIndex, &pSessionControl))) {

			CComPtr<IAudioSessionControl2> pAudioSessionControl2;
			if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (VOID**)&pAudioSessionControl2))) {

				DWORD id;
				if (SUCCEEDED(pAudioSessionControl2->GetProcessId(&id))) {

					if (id == PID) {
						if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(IAudioMeterInformation), (VOID**)&Meter))) {
							
							CComPtr<ISimpleAudioVolume> pSimpleAudio;
							if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&pSimpleAudio))) {
								std::cout << ">> Muting PID <<";
								pSimpleAudio->SetMasterVolume(0, 0);
								pSimpleAudio->SetMute(true, 0);
							}
							
							std::cout << "Found meter for PID: " << PID << "\n";
							return;
						}
					}
				}
			}
		}
	}

	throw std::runtime_error("Didn't find our PID on the mixer (LOOP END) \n");
}

float TSoundReader::GetPeak() {
	float Value = -0.01;
	Meter->GetPeakValue(&Value);
	return Value;
}

bool TSoundReader_Init(TSoundReader* &ptr, DWORD PID)
{
	try
	{
		ptr = new TSoundReader(PID);
		return true;
	}
	catch (std::runtime_error e)
	{
		std::cout << "Failed to initialize TSoundReader: " << e.what() << "\n";
		CoUninitialize();
		return false;
	}	
}

void TSoundReader_Free(TSoundReader* &ptr)
{
	CoUninitialize();
	delete ptr;
	ptr = NULL;
}

float TSoundReader_GetPeak(TSoundReader* &ptr)
{
	return ptr->GetPeak();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		module = hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



