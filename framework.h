#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include "SDK/SDK.hpp"
using namespace SDK;
#include "Minhook.h"
#pragma(lib, "minhook.lib")

inline uintptr_t ImageBase = uintptr_t(GetModuleHandle(0));
inline UFortEngine* GEngine = *(UFortEngine**)(ImageBase + 0x8155E78);

inline void (*StartAircraftPhase)(AFortGameModeAthena* GameMode, char a2) = decltype(StartAircraftPhase)(ImageBase + 0x18F9BB0);
static UObject* (*StaticLoadObjectOG)(UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation, void*) = decltype(StaticLoadObjectOG)(__int64(GetModuleHandleW(0)) + 0x2E1CD60);

template<typename T>
inline T* StaticLoadObject(std::string name)
{
    auto Name = std::wstring(name.begin(), name.end()).c_str();
    auto Object = (T*)StaticLoadObjectOG(T::StaticClass(), nullptr, Name, nullptr, 0, nullptr, false, nullptr);

    return Object;
}

inline UWorld* GetWorld()
{
	return GEngine->GameViewport->World;
}

inline void Hook(uintptr_t Address, void* Hook, void** OG)
{
	MH_CreateHook(LPVOID(Address), Hook, OG);
}

template<typename T>
inline T* SpawnActor(FTransform Transform = {})
{
    AActor* Start = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), T::StaticClass(), Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, nullptr);
    return (T*)UGameplayStatics::FinishSpawningActor(Start, Transform);
}

inline UFortPlaylistAthena* CurrentPlaylist()
{
    return ((AFortGameStateAthena*)UWorld::GetWorld()->GameState)->CurrentPlaylistInfo.BasePlaylist;
}

inline BYTE* __fastcall ChangeGameSessionId()
{
	return nullptr;
}

inline __forceinline void PatchByte(uintptr_t ptr, uint8_t byte)
{
    DWORD og;
    VirtualProtect(LPVOID(ptr), 1, PAGE_EXECUTE_READWRITE, &og);
    *(uint8_t*)(ptr) = byte;
    VirtualProtect(LPVOID(ptr), 1, og, &og);
}