#include "misc.h"
#include "PE.h"
#include "Gamemode.h"

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();
    FILE* I = nullptr;
    freopen_s(&I, "CONOUT$", "w+", stdout);
    SetConsoleTitleA("Shadow - 12.61");
    
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);

    Hooks::AttachHook(ImageBase + Offsets::ProcessEvent, ProcessEventHook);

    Hooks::AttachHook(ImageBase + 0x46FAA40, ReadyToStartMatchHook);

    Misc::DumpFunctions();

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

