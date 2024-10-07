#include "framework.h"
#include "GameMode.h"
#include "World.h"
#include "Abilities.h"
#include "Player.h"
#include "Bots.h"

bool KickPlayer() {
    std::cout << "KickPlayer" << std::endl;
    return true;
}

static void (*ServerReplicateActors)(void*) = decltype(ServerReplicateActors)(__int64(GetModuleHandleW(0)) + 0x1023F60);

void (*TickFlushOG)(UNetDriver* Driver);
void TickFlushHook(UNetDriver* Driver)
{
    if (Driver && Driver->ReplicationDriver && Driver->ClientConnections.Num() > 0) {
        ServerReplicateActors(Driver->ReplicationDriver);
    }

    if (GetAsyncKeyState(VK_INSERT)) {
        StartAircraftPhase((AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode, 0);
        Sleep(1000);
    }

    if (GetAsyncKeyState(VK_F3) && Driver) {
        
        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        for (auto& CC : Driver->ClientConnections) {
            GiveAccolade((AFortPlayerControllerAthena*)CC->PlayerController, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Bronze.AccoladeId_014_Elimination_Bronze"));
        }
    }

    return TickFlushOG(Driver);
}

bool rettrue()
{
    return true;
}

bool retfalse()
{
    return false;
}

void* (*DispatchRequestOG)(UObject*, __int64, __int64);
void* DispatchRequestHook(UMcpProfileGroup* a1, __int64 a2, __int64 a3) {
    std::cout << "DispatchRequest: " << __int64(a1->VTable) - ImageBase << std::endl;
    return DispatchRequestOG(a1, a2, 3);
}

void (*PE_OG)(UObject*, UFunction* Function, void* Params);
void PE_Hook(UObject* Object, UFunction* Function, void* Params) {
    
    if (Object && Function) {
        std::string FuncName = Function->GetName();
        std::string ObjectClassName = Object->Class->GetName();
        if (FuncName.contains("Respawn") || FuncName.contains("Resurrect")) {
            std::cout << "FuncName: " << Function->GetName() << std::endl;
        }
    }

    return PE_OG(Object, Function, Params);
}


DWORD WINAPI Main(LPVOID)
{
    AllocConsole();
    FILE* File;
    freopen_s(&File, "CONOUT$", "w+", stdout);
    SetConsoleTitleA("Reverse");
    MH_Initialize();
    Sleep(5000);

    GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), bCreative ? L"open Creative_NoApollo_Terrain" : L"open Apollo_Terrain", nullptr);


    Hook(ImageBase + 0x4640A30, GameMode::ReadyToStartMatchHook, nullptr);
    Hook(ImageBase + 0x45c9d90, GetNetMode, nullptr);
    //Hook(ImageBase + 0x3eb6780, GetNetMode, nullptr);
    //Hook(ImageBase + 0x1e23840, ChangeGameSessionId, nullptr);
    Hook(ImageBase + 0x4155600, KickPlayer, nullptr);
    Hook(ImageBase + 0x18F6250, GameMode::SpawnDefaultPawnFor, nullptr);
    Hook(ImageBase + 0x42C3ED0, TickFlushHook, (void**)&TickFlushOG);
    Hook(ImageBase + 0x108d740, DispatchRequestHook, (void**)&DispatchRequestOG);
    Hook(ImageBase + 0x2e13bf0, PE_Hook, (void**)&PE_OG);
    Hook(ImageBase + 0x19E9B10, SpawnBot, (void**)&SpawnBot_OG);
    Hook(ImageBase + 0x2683f80, OnDamageServer, (void**)&OnDamageServer_OG);
    Hook(ImageBase + 0x29B5C80, ClientOnPawnDied, (void**)&ClientOnPawnDied_OG);
    Hook(ImageBase + 0x1F96650, CompletePickupAnimation, (void**)&CompletePickupAnimation_OG);
    Hook(ImageBase + 0x1A6D300, sub_1A6D300_Hook, (void**)&sub_1A6D300);
    Hook(ImageBase + 0x1A91DC0, sub_1A91DC0_Hook, (void**)&sub_1A91DC0);
    Hook(ImageBase + 0x18FD350, SetZoneToIndex, (void**)&SetZoneToIndexOG);
    //Hook(ImageBase + 0x230C210, SetHasFinishedLoading, (void**)&SetHasFinishedLoading_OG);
    //Hook(ImageBase + 0x18E6B20, GameMode::PickTeamHook, nullptr);

    

    std::vector<uint64_t> NullFuncs = { ImageBase + 0x3ca10c0, ImageBase + 0x2d95e00, ImageBase + 0x3262100, ImageBase + 0x1e23840, ImageBase + 0x2d95dc0 };
    std::vector<uint64_t> RetTrueFuncs = { ImageBase + 0x4155600, ImageBase + 0x2DBCBA0 };

    for (auto& NullFunc : NullFuncs) PatchByte(NullFunc, 0xC3);
    for (auto& RetTrue : RetTrueFuncs) Hook(RetTrue, rettrue, nullptr);

    for (int i = 0; i < UObject::GObjects->Num(); i++) {
        UObject* Object = UObject::GObjects->GetByIndex(i);

        if (!Object) continue;

        if (Object->IsA(UAbilitySystemComponent::StaticClass())) {
            void** VFT = Object->VTable;

            if (!VFT) continue;

            DWORD Word;
            VirtualProtect(VFT + 0xfa, 8, PAGE_EXECUTE_READWRITE, &Word);
            VFT[0xfa] = Abilities::InternalServerTryActivateAbilityHook;
            DWORD NEW;
            VirtualProtect(VFT + 0xfa, 8, Word, &NEW);
        }
    }

    
    //ServerReadyToStartMatch_OG = decltype(ServerReadyToStartMatch_OG)(AFortPlayerControllerAthena::GetDefaultObj()->VTable[0x269]);
    DWORD F;
    VirtualProtect(AFortPlayerControllerAthena::GetDefaultObj()->VTable[0x10D], 8, PAGE_EXECUTE_READWRITE, &F);
    AFortPlayerControllerAthena::GetDefaultObj()->VTable[0x10D] = ServerAcknowledgePossesion;
    DWORD a;
    VirtualProtect(AFortPlayerControllerAthena::GetDefaultObj()->VTable[0x10D], 8, F, &a);

    MH_EnableHook(MH_ALL_HOOKS);

    uint64_t GIsClient = ImageBase + 0x804b659;
    *(bool*)GIsClient = false;

    DWORD d;
    VirtualProtect(UFortControllerComponent_Aircraft::GetDefaultObj()->VTable[0x89], 8, PAGE_EXECUTE_READWRITE, &d);
    UFortControllerComponent_Aircraft::GetDefaultObj()->VTable[0x89] = ServerAttemptAircraftJump;
    DWORD s;
    VirtualProtect(UFortControllerComponent_Aircraft::GetDefaultObj()->VTable[0x89], 8, d, &s);

    DWORD g;
    VirtualProtect(AFortPlayerControllerAthena::GetDefaultObj()->VTable[0x20D], 8, PAGE_EXECUTE_READWRITE, &g);
    AFortPlayerControllerAthena::GetDefaultObj()->VTable[0x20D] = ServerExecuteInventoryItem;
    DWORD f;
    VirtualProtect(AFortPlayerControllerAthena::GetDefaultObj()->VTable[0x20D], 8, g, &f);

    VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x21D, ServerAttemptInventoryDrop, (void**)&ServerAttemptInventoryDrop_OG);

    VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x269, ServerReadyToStartMatch, (void**)&ServerReadyToStartMatch_OG);

    //VFTHook(AFortGameModeAthena::GetDefaultObj()->VTable, 0xCE, GameMode::HandleStartingNewPlayer, (void**)GameMode::HandleStartingNewPlayer_OG);

    VFTHook(UNavigationSystemV1::GetDefaultObj()->VTable, 0x5B, rettrue, nullptr);

    VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x230, ServerCreateBuildingActor, (void**)&ServerCreateBuildingActor_OG);

    VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x237, ServerBeginEditingBuildingActor, (void**)&ServerBeginEditingBuildingActor_OG);

    VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x232, ServerEditBuildingActor, nullptr);

    VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x235, ServerEndEditingBuildingActor, nullptr);

    VFTHook(AFortPlayerPawnAthena::GetDefaultObj()->VTable, 0x1EA, ServerHandlePickup, nullptr);

    VFTHook(UFortControllerComponent_Interaction::GetDefaultObj()->VTable, 0x8B, ServerAttemptInteract, (void**)&ServerAttemptInteract_OG);

    VFTHook(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x4F0, ServerClientIsReadyToRespawn, (void**)&ServerClientIsReadyToRespawn_OG);

    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogOnlineGame VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogOnlineParty VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogOnlineSession VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogOnlineTournament VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogParty VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogLevel VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogAthenaNavMesh VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogAIHotSpot VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortAIDebug VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortBotMission VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogDeimosAIPerception VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogAthenaAI VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogAthenaPathing VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogSkyTechGrantItem VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogAISpawnerData VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogAthenaBots VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogMutatorAI VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogDadBro VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortLoot VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortTeams VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortQuest VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortPlayerPawn VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortNavigation VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortSquadSlotManager VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortAIDirector VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortAI VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogDadBro VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortPlacement VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogAIModule VeryVerbose", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"log LogFortMutatorInventoryOverride VeryVerbose", nullptr);


    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

