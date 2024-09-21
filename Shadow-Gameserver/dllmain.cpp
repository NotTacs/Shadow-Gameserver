#include "framework.h"
#include "Gamemode.h"

DWORD Main(LPVOID)
{
    AllocConsole();
    FILE* FILE;
    freopen_s(&FILE, "CONOUT$", "w+", stdout);
    SetConsoleTitleA("Shadow 12.41");
    MH_Initialize();
    Sleep(5000);
    
    Hooks::AttachHook(ImageBase + 0x4640A30, ReadyToStartMatch_Hook);
    Hooks::AttachHook(ImageBase + 0x45CBB10, Test_Hook, Test_OG);

    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);

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
        CreateThread(nullptr, 0, Main, nullptr, 0, 0);
        break;
    }
    return 1;
}

