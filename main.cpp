#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <memory>
#include <stdexcept>
#include <system_error>

#define SYSTEM_ERROR(e, m) (std::system_error((e), std::system_category(), (m)))

DISPLAYCONFIG_TOPOLOGY_ID GetCurrentDisplayTopology()
{
	DISPLAYCONFIG_TOPOLOGY_ID topology;
	LONG queryResult = ERROR_INSUFFICIENT_BUFFER;
	while (ERROR_INSUFFICIENT_BUFFER == queryResult) {
		UINT32 pathCount;
		UINT32 modeInfoCount;
		LONG bufferSizeResult = GetDisplayConfigBufferSizes(QDC_DATABASE_CURRENT, &pathCount, &modeInfoCount);
		if (ERROR_SUCCESS != bufferSizeResult) {
			throw SYSTEM_ERROR(static_cast<int>(bufferSizeResult), "GetDisplayConfigBufferSizes");
		}
		auto paths = std::make_unique<DISPLAYCONFIG_PATH_INFO[]>(static_cast<size_t>(pathCount));
		auto modeInfos = std::make_unique<DISPLAYCONFIG_MODE_INFO[]>(static_cast<size_t>(modeInfoCount));
		queryResult = QueryDisplayConfig(
			QDC_DATABASE_CURRENT, &pathCount, paths.get(), &modeInfoCount, modeInfos.get(), &topology);
	}
	if (ERROR_SUCCESS != queryResult) {
		throw SYSTEM_ERROR(static_cast<int>(queryResult), "QueryDisplayConfig");
	}
	return topology;
}

void SetCurrentDisplayTopology(DISPLAYCONFIG_TOPOLOGY_ID topology)
{
	LONG result = SetDisplayConfig(
		0, nullptr, 0, nullptr, SDC_APPLY | static_cast<UINT32>(topology));
	if (ERROR_SUCCESS != result) {
		throw SYSTEM_ERROR(static_cast<int>(result), "SetDisplayConfig");
	}
}

void SwapCurrentTopology()
{
	constexpr auto INTERNAL = DISPLAYCONFIG_TOPOLOGY_INTERNAL;
	constexpr auto EXTEND = DISPLAYCONFIG_TOPOLOGY_EXTEND;
	DISPLAYCONFIG_TOPOLOGY_ID topology = GetCurrentDisplayTopology();
	DISPLAYCONFIG_TOPOLOGY_ID newTopology = (topology == INTERNAL) ? EXTEND : INTERNAL;
	SetCurrentDisplayTopology(newTopology);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR pCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	try {
		SwapCurrentTopology();
	}
	catch (std::exception& e) {
		MessageBoxA(nullptr, e.what(), "DisplayToggle error", MB_OK | MB_ICONERROR);
		return 1;
	}
	return 0;
}