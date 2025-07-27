#pragma once
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "SDK/SDK.hpp"
using namespace SDK;
#include "Minhook.h"


static bool bCreative = false;

inline uintptr_t ImageBase = uintptr_t(GetModuleHandle(0));
inline UFortEngine* GEngine = *(UFortEngine**)(ImageBase + 0x8155E78);


inline ANavigationData* (*CreateNavigationDataInstanceInLevel)(UNavigationSystemV1* System, const FNavDataConfig& NavConfig, ULevel* SpawnLevel) = decltype(CreateNavigationDataInstanceInLevel)(ImageBase + 0x4808690);
inline void (*RebuildAll)(UNavigationSystemV1* System, bool bIsLoadTime) = decltype(RebuildAll)(ImageBase + 0x48201A0);
inline UUserWidget* (*CreateWidgetInstance)(APlayerController& OwnerPC, TSubclassOf<UUserWidget> Class, FName WidgetName) = decltype(CreateWidgetInstance)(ImageBase + 0x3C8ECE0);
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

inline std::string LootTierGroupModifcation(const std::string& input) {
    if (input.contains("Athena")) return input;

    size_t pos = input.find('_');

    if (pos != std::string::npos && input.substr(0, pos) == "Loot") {
        return "Loot_Athena" + input.substr(pos + 1);
    }

    return input;
}

inline std::string ToString2(FName Name)  {
    FString F = UKismetStringLibrary::Conv_NameToString(Name);
    return F.ToString();
}

template<typename T>
T* StaticLoadObject(std::string name)
{
    T* Object = StaticFindObject<T>(name);

    if (!Object)
    {
        auto Name = std::wstring(name.begin(), name.end()).c_str();
        Object = (T*)StaticLoadObjectOG(T::StaticClass(), nullptr, Name, nullptr, 0, nullptr, false, nullptr);
    }

    return Object;
}

inline UWorld* GetWorld()
{
    return *reinterpret_cast<UWorld**>(ImageBase + 0x8158708);
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

inline __forceinline void PatchByte16(uintptr_t ptr, uint16_t byte)
{
    DWORD og;
    VirtualProtect(LPVOID(ptr), 1, PAGE_EXECUTE_READWRITE, &og);
    *(uint16_t*)(ptr) = byte;
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

inline __int64 (*GameStateShit)() = decltype(GameStateShit)(ImageBase + 0x2857EC0);

static FVector ZoneLoc;

inline bool bLateGame = false;

inline void DumpAllMetalCards() {
    
}

inline bool UWorld_Listen() {
    static bool bListen = false;
    if (!bListen) {
        auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;
        auto GameMode = (AFortGameModeAthena*)GetWorld()->AuthorityGameMode;
        
        bListen = true;
    }
    
    return true;
}

inline void (*SetZoneToIndexOG)(AFortGameModeAthena* GameMode, int a2);
inline void SetZoneToIndex(AFortGameModeAthena* GameMode, int a2) {
    auto Gamestate = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
    auto Indicator = GameMode->SafeZoneIndicator;
    printf("SetZoneToIndexCalled \n");
    static int LateGamePhase = 2;

    

    if (bLateGame)
    {

        ZoneLoc = GameMode->SafeZoneLocations[4];
        Indicator->NextCenter.X = ZoneLoc.X;
        Indicator->NextCenter.Y = ZoneLoc.Y;
        Indicator->NextCenter.Z = ZoneLoc.Z;

        std::cout << "NextCenter: ("
            << Indicator->NextCenter.X << ", "
            << Indicator->NextCenter.Y << ", "
            << Indicator->NextCenter.Z << ")" << std::endl;

        static FVector_NetQuantize100 ZoneLocQuan = FVector_NetQuantize100{ ZoneLoc.X, ZoneLoc.Y, ZoneLoc.Z };

        SetZoneToIndexOG(GameMode, a2);

        if (LateGamePhase == 2 || LateGamePhase == 3)
        {
            UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"skipshrinksafezone", nullptr);
            printf("Phase %d: Safe zone shrink skipped.\n", LateGamePhase);
        }

        if (LateGamePhase == 3)
        {
            if (Indicator)
            {
                Indicator->SafeZoneStartShrinkTime = Gamestate->GetServerWorldTimeSeconds();
                Indicator->SafeZoneFinishShrinkTime = Gamestate->GetServerWorldTimeSeconds() + 0.2;

                printf("Phase 3: SafeZoneStartShrinkTime: %f\n", Indicator->SafeZoneStartShrinkTime);
                printf("Phase 3: SafeZoneFinishShrinkTime: %f\n", Indicator->SafeZoneFinishShrinkTime);
                printf("Phase 3: NextRadius: %f, LastRadius: %f\n", Indicator->NextRadius, Indicator->LastRadius);
            }
        }

        if (LateGamePhase == 4)
        {
            Indicator->NextCenter = ZoneLocQuan;
            Indicator->NextRadius = 5000.f;
            Indicator->LastRadius = 10000.f;

            printf("Phase 4: NextRadius: %f, LastRadius: %f\n", Indicator->NextRadius, Indicator->LastRadius);
        }

        if (LateGamePhase == 5)
        {
            Indicator->NextCenter = ZoneLocQuan;
            Indicator->NextRadius = 2500.f;
            Indicator->LastRadius = 5000.f;

            printf("Phase 5: NextRadius: %f, LastRadius: %f\n", Indicator->NextRadius, Indicator->LastRadius);
        }

        if (LateGamePhase == 6)
        {
            Indicator->NextCenter = ZoneLocQuan;
            Indicator->NextRadius = 1650.f;
            Indicator->LastRadius = 2500.f;

            printf("Phase 6: NextRadius: %f, LastRadius: %f\n", Indicator->NextRadius, Indicator->LastRadius);
        }

        if (LateGamePhase == 7)
        {
            Indicator->NextCenter = ZoneLocQuan;
            Indicator->NextRadius = 1090.f;
            Indicator->LastRadius = 1650.f;

            printf("Phase 7: NextRadius: %f, LastRadius: %f\n", Indicator->NextRadius, Indicator->LastRadius);
        }

        LateGamePhase++;

        return;
    }
    return SetZoneToIndexOG(GameMode, a2);
}

inline void (*InitForWorld_OG)(UAthenaNavSystem* NavigationSystem, UWorld* World, EFNavigationSystemRunMode NavigationSystemRunMode);
inline void InitForWorld(UAthenaNavSystem* NavigationSystem, UWorld* World, EFNavigationSystemRunMode NavigationSystemRunMode) {
    NavigationSystem->bAutoCreateNavigationData = true;
    //NavigationSystem->DataGatheringMode = ENavDataGatheringModeConfig::Instant;
    printf("NavMesh");

	printf("NavSystem: %s\n", NavigationSystem->GetName().c_str());

    for (auto& Agents : NavigationSystem->SupportedAgents)
    {
        std::cout << "AgentName: " << Agents.Name.ToString() << std::endl;
        std::cout << "AgentRadius: " << Agents.AgentRadius << std::endl;
        std::cout << "AgentHeight: " << Agents.AgentHeight << std::endl;
    }

    return InitForWorld_OG(NavigationSystem, World, NavigationSystemRunMode);
}


inline UNavigationSystemBase* (*CreateAndConfigureNavigationSystemOG)(UAthenaNavSystemConfig* System, UWorld* World);
inline UNavigationSystemBase* CreateAndConfigureNavigationSystem(UAthenaNavSystemConfig* System, UWorld* World)
{
	printf("CreateAndConfigureNavigationSystem Called\n");
    System->bAutoSpawnMissingNavData = true;
    System->bPrioritizeNavigationAroundSpawners = true;
    return CreateAndConfigureNavigationSystemOG(System, World);
}


inline void (*sub_1A91DC0)(UObject* Mutator, int a2, __int64 a3);
inline void sub_1A91DC0_Hook(AFortAthenaMutator_Mash* Mutator, int a2, UFortDifficultyEncounterSettings* a3) {

    printf("Called");

    auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
    auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

    auto System = (UAthenaAISystem*)UWorld::GetWorld()->AISystem;

    std::cout << "MutatorClass: " << Mutator->Class->GetName() << std::endl;

    auto ObjectVFT = a3->VTable;

    std::cout << std::hex << __int64(ObjectVFT) - ImageBase;

    auto Shit = (__int64*)a3;

    return sub_1A91DC0(Mutator, a2, *Shit);
}

inline void (*sub_1A6D300)(UObject* a1) = decltype(sub_1A6D300)(ImageBase + 0x1A6D300);
inline void sub_1A6D300_Hook(UObject* a1) {

    printf("sub_1A6D300 Called");

    auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
    auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

    std::cout << "Class: " << a1->Class->GetFullName() << std::endl;

    auto G = (AFortAthenaMutator_DadBro*)GameState->GetMutatorByClass(GameMode, AFortAthenaMutator_DadBro::StaticClass());
    G->DadBroSpawnLocation.Z = -193.048096f;
    FTransform Transform{};
    Transform.Translation = G->DadBroSpawnLocation;
    Transform.Rotation = ConvertRotToQuat(G->GetDesiredDadBroRotation());
    Transform.Scale3D = FVector(1, 1, 1);
    UClass* Class = StaticLoadObject<UClass>("/Game/Athena/DADBRO/DADBRO_Pawn.DADBRO_Pawn_C");
    G->DadBroPawn = SpawnActor<AFortAIPawn>(Transform, nullptr, Class);
    G->DadBroCodeState = EDadBroState::Active;
    G->OnRep_DadBroPawn();
    G->OnRep_DadBroCodeState();
    G->HandleAISpawned(G->DadBroEncounterInstance, G->DadBroPawn);

    //This could be better coded, I need to figure out which one is supposed to be the SpawnDadBro Function and hook that

    return sub_1A6D300(a1);
}


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

inline float GetRewardAmount(UFortAccoladeItemDefinition* Def) {
    return FindCurveTable(Def->XpRewardAmount.Curve.CurveTable, Def->XpRewardAmount.Curve.RowName);
}

inline void GiveMedal(AFortPlayerControllerAthena* Controller) {
    
}

inline void GiveAccolade(AFortPlayerControllerAthena* Controller, UFortAccoladeItemDefinition* Def) {
    FAthenaAccolades Accolades{};
    Accolades.AccoladeDef = Def;
    Accolades.Count = 1;
    Accolades.TemplateId = Def->GetName();
    Controller->XPComponent->PlayerAccolades.Add(Accolades);
    Controller->XPComponent->MedalsEarned.Add(Def);
    FCardSlotMedalData MD{};
    MD.AccoladeForSlot = Def;
    MD.bLoadedFromMcp = false;
    MD.bPunched = true;
    MD.SlotIndex = 0;
    Controller->XPComponent->LocalPunchCardMedals.Add(MD);
    //Controller->XPComponent->ClientMedalsRecived(Controller->XPComponent->PlayerAccolades);
    FXPEventInfo Info{};
    Info.Accolade = UKismetSystemLibrary::GetPrimaryAssetIdFromObject(Def);
    Info.EventName = Def->Name;
    Info.EventXpValue = GetRewardAmount(Def);
    Info.Priority = Def->GetPriority();
    Info.SimulatedText = Def->Description;
    Controller->XPComponent->OnXPEvent(Info);
}

enum class EInternalObjectFlags : int32
{
    None = 0,

    // NOTE that the minimum flag bit index is currently 14. See EInternalObjectFlags_MinFlagBitIndex and UE_ENABLE_FUOBJECT_ITEM_PACKING in UObjectArray.h

    ReachabilityFlag0 = 1 << 14, ///< One of the flags used by Garbage Collector to determine UObject's reachability state
    ReachabilityFlag1 = 1 << 15, ///< One of the flags used by Garbage Collector to determine UObject's reachability state
    ReachabilityFlag2 = 1 << 16, ///< One of the flags used by Garbage Collector to determine UObject's reachability state

    AutoRTFMConstructionAborted = 1 << 17, //< Object was constructed in an AutoRTFM transaction that has been aborted
    Remote = 1 << 18, //< Object is no longer owned by this process
    RemoteReference = 1 << 19, //< Object referenced by remote process
    LoaderImport = 1 << 20, ///< Object is ready to be imported by another package during loading
    Garbage = 1 << 21, ///< Garbage from logical point of view and should not be referenced. This flag is mirrored in EObjectFlags as RF_Garbage for performance
    AsyncLoadingPhase1 = 1 << 22, ///< Object is being asynchronously loaded.
    ReachableInCluster = 1 << 23, ///< External reference to object in cluster exists
    ClusterRoot = 1 << 24, ///< Root of a cluster
    Native = 1 << 25, ///< Native (UClass only). 
    Async = 1 << 26, ///< Object exists only on a different thread than the game thread.
    AsyncLoadingPhase2 = 1 << 27, ///< Object is being asynchronously loaded.
    Unreachable = 1 << 28, ///< Object is not reachable on the object graph.
    RefCounted = 1 << 29, ///< Object currently has ref-counts associated with it.
    RootSet = 1 << 30, ///< Object will not be garbage collected, even if unreferenced.
    PendingConstruction = 1 << 31, ///< Object didn't have its class constructor called yet (only the UObjectBase one to initialize its most basic members)

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // DO NOT ADD composite flags to EInternalObjectFlags. Debugger visualisations have trouble displaying composite flag values as text.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
};

inline const wchar_t* (*GetCommandLineOG)();
inline const wchar_t* GetCommandLineHook()
{
    return GetCommandLine();
}