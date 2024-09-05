#pragma once
#include <windows.h>
#include <thread>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdint>
#include "detours/detours.h"
#include "SDK/SDK.hpp"
using namespace SDK;
using namespace UC;

inline std::vector<std::pair<uintptr_t, void*>> DetourHooks;

inline std::vector<std::pair<UFunction*, void*>> Functions;

inline TSet<std::pair<UFunction*, void*>> UniqueFunctions;

inline uintptr_t ImageBase = uintptr_t(GetModuleHandle(0));

namespace Hooks
{
	void DetachHooks();

	void AttachHook(uintptr_t Address, PVOID Hook);
}

namespace Misc
{
	void KillServer();

	std::vector<UObject*> GetObjectsOfClass(UClass* Class);

	void DumpFunctions();

	int ConvertToFunc(int Index, void** VFT);

	int FuncToIndex(uintptr_t offset, void** VFT);
}