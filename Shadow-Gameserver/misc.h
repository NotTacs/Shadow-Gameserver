#pragma once
#include <windows.h>
#include <thread>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
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

namespace Imports
{
	inline UNetDriver* (*CreateNetDriver)(UEngine* Engine, UWorld* InWorld, FName NetDriverDefinition) = decltype(CreateNetDriver)(ImageBase + 0x462D730);
	inline bool (*InitListen)(UNetDriver*, UWorld* InNotify, FURL& ListenURL, bool bReuse, FString& Error) = decltype(InitListen)(ImageBase + 0xD96670);
	inline void (*SetWorld)(UNetDriver*, UWorld* World) = decltype(SetWorld)(ImageBase + 0x437c600);
}

namespace Hooks
{
	void DetachHooks();

	void AttachHook(uintptr_t Address, PVOID Hook);

	int retzerohook();

	int retonehook();
}

namespace Misc
{
	void KillServer();

	std::vector<UObject*> GetObjectsOfClass(UClass* Class);

	void DumpFunctions();

	int ConvertToFunc(int Index, void** VFT);

	int FuncToIndex(uintptr_t offset, void** VFT);

	void decToHex(int decimalValue);
}