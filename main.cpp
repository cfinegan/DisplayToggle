#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <stdexcept>
#include <system_error>

#define SYSTEM_ERROR(e, m) (std::system_error((e), std::system_category(), (m)))

struct TopologyPair {
	DISPLAYCONFIG_TOPOLOGY_ID primary;
	DISPLAYCONFIG_TOPOLOGY_ID secondary;
};

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

// The primary topology is chosen if the current topology is any other than the primary.
// The secondary topology is chosen if the current topology is the primary.
void SwapCurrentTopology(DISPLAYCONFIG_TOPOLOGY_ID primary, DISPLAYCONFIG_TOPOLOGY_ID secondary)
{
	DISPLAYCONFIG_TOPOLOGY_ID current = GetCurrentDisplayTopology();
	SetCurrentDisplayTopology((current == primary) ? secondary : primary);
}

DISPLAYCONFIG_TOPOLOGY_ID StringToTopology(const char* topology) 
{
	if (!_stricmp(topology, "internal")) {
		return DISPLAYCONFIG_TOPOLOGY_INTERNAL;
	}
	else if (!_stricmp(topology, "clone")) {
		return DISPLAYCONFIG_TOPOLOGY_CLONE;
	}
	else if (!_stricmp(topology, "extend")) {
		return DISPLAYCONFIG_TOPOLOGY_EXTEND;
	}
	else if (!_stricmp(topology, "external")) {
		return DISPLAYCONFIG_TOPOLOGY_EXTERNAL;
	}
	else {
		std::string msg("Invalid topology specified on command line: ");
		msg += topology;
		throw std::runtime_error(msg);
	}
}

TopologyPair ArgsToTopologies(int argc, char** argv)
{
	if (argc < 2) {
		return TopologyPair{ DISPLAYCONFIG_TOPOLOGY_INTERNAL, DISPLAYCONFIG_TOPOLOGY_EXTEND };
	}
	else if (argc == 3) {
		return TopologyPair{ StringToTopology(argv[1]), StringToTopology(argv[2]) };
	}
	else {
		std::string msg("Invalid usage:\n\n");
		msg += GetCommandLineA();
		msg += "\n\nProgram should be run with 2 arguments, or none";
		throw std::runtime_error(msg);
	}
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try {
		TopologyPair topologies = ArgsToTopologies(__argc, __argv);
		SwapCurrentTopology(topologies.primary, topologies.secondary);
	}
	catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "DisplayToggle error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;

	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
}
