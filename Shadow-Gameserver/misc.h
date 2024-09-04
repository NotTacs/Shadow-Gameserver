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


inline std::vector<std::pair<uintptr_t, void*>> DetourHooks;

inline uintptr_t ImageBase = uintptr_t(GetModuleHandle(0));

namespace Hooks
{
	void DetachHooks();

	void AttachHook(uintptr_t Address, PVOID Hook, PVOID OG = nullptr);
}

namespace Misc
{
	void KillServer();

	std::vector<UObject*> GetObjectsOfClass(UClass* Class);

	void DumpFunctions();
}


