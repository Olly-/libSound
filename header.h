#include <windows.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <atlbase.h>

#define DLL_FUNC __declspec(dllexport)
extern HMODULE module;

static const char* PascalExports[] =
{
	"TSoundReader_Init", "function TSoundReader.Init(PID: DWord; Window: DWord): Boolean;",
	"TSoundReader_GetPeak", "function TSoundReader.GetPeak(): Integer;",
	"TSoundReader_WaitForPeak", "function TSoundReader.WaitForPeak(MinPeakValue: Integer; WaitFor: Integer): Boolean;",
	"TSoundReader_Free", "procedure TSoundReader.Free();"
};

static const char* PascalTypes[] =
{
	"TSoundReader", "type TObject;"    
};

static const long int PascalExportCount = sizeof(PascalExports) / (sizeof(PascalExports[0]) * 2);
static const long int PascalTypeCount = sizeof(PascalTypes) / (sizeof(PascalTypes[0]) * 2);

class TSoundReader {
	CComPtr<IAudioMeterInformation> Meter;
public:
	TSoundReader(DWORD PID);
	int GetPeak();
	bool WaitForPeak(int MinValue, int WaitFor);
};

#ifdef __cplusplus
extern "C"
{
#endif

	DLL_FUNC int GetPluginABIVersion();
	DLL_FUNC int GetFunctionCount();
	DLL_FUNC int GetFunctionInfo(int Index, void** Address, char** Definition);
	DLL_FUNC int GetTypeCount();
	DLL_FUNC int GetTypeInfo(int Index, char** Type, char** Definition);

	DLL_FUNC bool TSoundReader_Init(TSoundReader* &ptr, DWORD PID, DWORD Window);
	DLL_FUNC int TSoundReader_GetPeak(TSoundReader* &ptr);
	DLL_FUNC bool TSoundReader_WaitForPeak(TSoundReader* &ptr, int MinValue, int WaitFor);
	DLL_FUNC void TSoundReader_Free(TSoundReader* &ptr);

#ifdef __cplusplus
}
#endif
