#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include "SDK/SDK.hpp"
using namespace SDK;
#include "Minhook.h"

inline uintptr_t ImageBase = uintptr_t(GetModuleHandle(0));
inline UFortEngine* GEngine = *(UFortEngine**)(ImageBase + 0x8155E78);

inline double (*InitQuests)(UFortQuestManager*, unsigned __int8 a2, char a3) = decltype(InitQuests)(ImageBase + 0x23A5B80);
inline void (*Sub_16BDED0)(UNavigationSystemV1* System) = decltype(Sub_16BDED0)(ImageBase + 0x16BDED0);
inline void (*Build)(UNavigationSystemV1* System) = decltype(Build)(ImageBase + 0x4804AA0);
inline void (*StartAircraftPhase)(AFortGameModeAthena* GameMode, char a2) = decltype(StartAircraftPhase)(ImageBase + 0x18F9BB0);
static UObject* (*StaticLoadObjectOG)(UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation, void*) = decltype(StaticLoadObjectOG)(__int64(GetModuleHandleW(0)) + 0x2E1CD60);
static UObject* (*StaticFindObjectOG)(UClass*, UObject* Package, const wchar_t* OrigInName, bool ExactClass) = decltype(StaticFindObjectOG)(__int64(GetModuleHandleW(0)) + 0x2E1C4B0);

template <typename T>
static T* StaticFindObject(std::string ObjectName)
{
    auto NameWStr = std::wstring(ObjectName.begin(), ObjectName.end()).c_str();

    return (T*)StaticFindObjectOG(T::StaticClass(), nullptr, NameWStr, false);
}

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
inline T* SpawnActor(FTransform Transform = {}, AActor* Owner = nullptr, UClass* OverrideClass = T::StaticClass())
{
    AActor* Start = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), OverrideClass, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner);
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

inline void sinCos(float* ScalarSin, float* ScalarCos, float Value)
{
    float quotient = (0.31830988618f * 0.5f) * Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - (2.0f * 3.1415926535897932f) * quotient;

    float sign;
    if (y > 1.57079632679f)
    {
        y = 3.1415926535897932f - y;
        sign = -1.0f;
    }
    else if (y < -1.57079632679f)
    {
        y = -3.1415926535897932f - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    *ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *ScalarCos = sign * p;
}

inline FQuat ConvertRotToQuat(const FRotator& Rot) {
    const float DEG_TO_RAD = 3.1415926535897932f / (180.f);
    const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
    float SP, SY, SR;
    float CP, CY, CR;

    sinCos(&SP, &CP, Rot.Pitch * DIVIDE_BY_2);
    sinCos(&SY, &CY, Rot.Yaw * DIVIDE_BY_2);
    sinCos(&SR, &CR, Rot.Roll * DIVIDE_BY_2);

    FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

inline void UFunctionHook(UFunction* Function, void* Detour, void** OG) {
    if (!Function) {
        printf("Invalid Pointer To Function");
        return;
    }

    if (OG) {
        *OG = Function->ExecFunction;
    }

    Function->ExecFunction = (UFunction::FNativeFuncPtr)Detour;
}

class FFrame {
public:
    uint8* GetLocals() {
        return *(uint8**)(uintptr_t(this) + 0x40);
    }
    void Step(UObject& Context, void* const Z_ParamResult) {
        void (*Step_OG)(__int64,UObject&, void* const) = decltype(Step_OG)(ImageBase + 0x2e1dd00);
        Step_OG(__int64(this),Context, Z_ParamResult);
    }
};


inline float FindCurveTable(UCurveTable* CurveTable, FName RowName) {
    if (!CurveTable) printf("No CurveTable Now");
    EEvaluateCurveTableResult Result;
    float Out;
    UDataTableFunctionLibrary::EvaluateCurveTableRow(CurveTable, RowName, 0.f, &Result, &Out, FString());
    if (Result == EEvaluateCurveTableResult::RowNotFound) {
        std::cout << "Very Proper" << std::endl;
    }

    return Out;
}

inline void VFTHook(void** VFT, int Index, void* Hook, void** OG) {

    if (OG)
        *OG = VFT[Index];

    DWORD S;
    VirtualProtect(VFT + Index, 8, PAGE_EXECUTE_READWRITE, &S);
    VFT[Index] = Hook;
    DWORD b;
    VirtualProtect(VFT + Index, 8, S, &b);
}