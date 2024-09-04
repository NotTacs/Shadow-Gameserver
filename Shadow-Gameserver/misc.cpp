#include "misc.h"

void Hooks::DetachHooks()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	for (auto& Hook : DetourHooks)
	{
		DetourDetach(&(PVOID&)Hook.first, Hook.second);
	}

	DetourTransactionCommit();
}

void Hooks::AttachHook(uintptr_t Address, PVOID Hook, PVOID OG)
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

void Misc::KillServer()
{
	UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"demostop", nullptr);
	exit(0);
}

std::vector<UObject*> Misc::GetObjectsOfClass(UClass* Class)
{
	std::vector<UObject*> Objects = {};

	for (int i = 0; i < UObject::GObjects->Num(); i++)
	{
		UObject* Object = UObject::GObjects->GetByIndex(i);
		if (Object->IsA(Class))
			Objects.push_back(Object);
	}

	return Objects;
}

void Misc::DumpFunctions()
{
	auto Functions = GetObjectsOfClass(UFunction::StaticClass());

	std::ofstream LogFile("Functions.txt");

	for (UObject* Object : Functions)
	{
		auto Function = reinterpret_cast<UFunction*>(Object);
		LogFile << Function->GetFullName() << "\n";
	}

	LogFile.close();
}