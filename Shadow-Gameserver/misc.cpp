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

void Hooks::AttachHook(uintptr_t Address, PVOID Hook)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

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

		if (!Object) continue;

		if (Object->IsA(Class))
			Objects.push_back(Object);
	}

	return Objects;
}

void Misc::DumpFunctions()
{
	auto Functions = Misc::GetObjectsOfClass(UFunction::StaticClass());
	std::ofstream LogFile("Functions.txt");
	for (UObject* Object : Functions)
	{
		auto Function = reinterpret_cast<UFunction*>(Object);
		LogFile << Function->GetFullName() << "\n";
	}

	LogFile.close();
}

int Misc::ConvertToFunc(int Index, void** VFT)
{
	auto SomeIndex = Index * 8;
	auto FuncOffset = SomeIndex + __int64(VFT);

	return FuncOffset;
}

int Hooks::retonehook()
{
	return 1;
}

int Hooks::retzerohook()
{
	return 0;
}

int Misc::FuncToIndex(uintptr_t Offset, void** VFT)
{
	int Vft = __int64(VFT) - ImageBase;

	auto smart = Offset - Vft;

	return smart;
}

void Misc::decToHex(int DecimalValue)
{
	std::cout << "Decimal: " << DecimalValue << " -> Hexadecimal: 0x" << std::hex << std::uppercase << DecimalValue << "\n";
}