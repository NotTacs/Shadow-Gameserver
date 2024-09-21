#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include "Minhook/MinHook.h"
#pragma comment(lib, "Minhook/MinHook.x64.lib")
#include "SDK/SDK.hpp"
using namespace SDK;

typedef unsigned long long QWORD;

inline uintptr_t ImageBase = uintptr_t(GetModuleHandle(0));

inline void (*SetWorld_OG)(UNetDriver*, UWorld*) = decltype(SetWorld_OG)(InSDKUtils::GetImageBase() + 0x42C2B20);

inline __int64 (*Test_OG)(__int64 a1);
inline __int64 __fastcall Test_Hook(__int64 a1)
{
	return 1;
	if (!&a1)
	{
		printf("NO NO No A1");

	}
	if (*(QWORD*)(*(QWORD*)(a1 + 0x10) + 0x88))
	{
		printf("Ok If I can do it then so can you.");

		return Test_OG(a1);
	}
}

namespace Misc
{
	std::vector<UObject*> GetObjectsOfClass(UClass* Class);
}

namespace Hooks
{
	void AttachHook(uintptr_t Address, void* Hook, PVOID OG = nullptr);
}