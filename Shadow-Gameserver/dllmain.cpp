#include "misc.h"
#include "PE.h"
#include "Gamemode.h"
#include "NetDriver.h"

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();
    FILE* I = nullptr;
    freopen_s(&I, "CONOUT$", "w+", stdout);
    SetConsoleTitleA("Shadow - 12.61");
    MH_Initialize();
    Sleep(5000);


    
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);

    for (auto& NullFunc : NullFuncs) Hooks::PatchByte(NullFunc, 0xC3);
    Hooks::AttachHook(ImageBase + 0x420f5d0, Hooks::retonehook);

    //Hooks::AttachHook(ImageBase + Offsets::ProcessEvent, ProcessEventHook);

    //Hooks::AttachHook(ImageBase + 0x420f5d0, Hooks::retzerohook);

    Hooks::AttachHook2(ImageBase + 0x194ccb0, ReadyToStartMatchHook, nullptr);

    //Hooks::AttachHook(ImageBase + 0x1957130, SpawnDefaultPawnForHook);

    //Hooks::AttachHook(ImageBase + 0x4683cd0, Hooks::retonehook); // GetNetMode

    //Hooks::AttachHook(ImageBase + 0x3f6a9e0, Hooks::retonehook);

    //Hooks::AttachHook2(ImageBase + 0x437d9b0, TickFlush, TickFlush_OG);

    //Misc::DumpFunctions();

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, Main, nullptr, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        Hooks::DetachHooks();
        break;
    }
    return TRUE;
}

