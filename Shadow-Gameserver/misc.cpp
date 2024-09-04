#include "misc.h"

void DetachHooks()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (auto& Hook : Hooks)
	{
		DetourDetach(&(PVOID&)Hook.first, Hook.second);
	}

	DetourTransactionCommit();
}

void AttachHook(uintptr_t Address, PVOID Hook, PVOID OG)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (OG)
	{
		OG = decltype(OG)(ImageBase + Address);
	}

	DetourAttach(&(PVOID&)Address, Hook);


	DetourTransactionCommit();
}