#include "framework.h"

std::vector<UObject*> Misc::GetObjectsOfClass(UClass* Class)
{
	std::vector<UObject*> Objects;
	for (int i = 0; i < UObject::GObjects->Num(); i++)
	{
		UObject* Object = UObject::GObjects->GetByIndex(i);

		if (!Object) continue;

		if (Object->IsA(Class)) Objects.push_back(Object);
	}

	return Objects;
}

void Hooks::AttachHook(uintptr_t Addr, void* Hook, PVOID OG)
{
	MH_STATUS Status1 = MH_CreateHook(LPVOID(Addr), Hook, &OG);
	MH_STATUS Status2 = MH_EnableHook(LPVOID(Addr));

	if (Status1 != MH_OK || Status2 != MH_OK)
	{
		printf("[MH]Failed to hook Address: " + ImageBase - Addr);
	}
}