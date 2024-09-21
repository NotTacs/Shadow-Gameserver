#include "framework.h"
#include "Gamemode.h"

bool KickPlayer()
{
    return false;
}

int rettruehook()
{
    return 1;
}
void (*TickFlush_OG)(UNetDriver*);
void TickFlushHook(UNetDriver* driver)
{
    if (driver->ReplicationDriver)
    {
        static void (*ServerReplicateActors)(UReplicationDriver*) = decltype(ServerReplicateActors)(__int64(GetModuleHandleW(0)) + 0x1023F60);
        ServerReplicateActors(driver->ReplicationDriver);
    }
    return TickFlush_OG(driver);
}


DWORD Main(LPVOID)
{
    AllocConsole();
    FILE* FILE;
    freopen_s(&FILE, "CONOUT$", "w+", stdout);
    SetConsoleTitleA("Shadow 12.41");
    MH_Initialize();
    Sleep(5000);
    
    Hooks::AttachHook(ImageBase + 0x4640A30, ReadyToStartMatch_Hook);
    Hooks::AttachHook(ImageBase + 0x4155600, KickPlayer);
    Hooks::AttachHook(ImageBase + 0x45C9D90, rettruehook);
    // Hooks::AttachHook(ImageBase + 0x45CBB10, Test_Hook, Test_OG);
    //Hooks::AttachHook(ImageBase + 0x42C3ED0, TickFlushHook, TickFlush_OG);
    Hooks::AttachHook(ImageBase + 0x18F6250, SpawnDefaultPawnFor_Hook);

    bool GIsClient = *(bool*)ImageBase + 0x804b659;
    GIsClient = false;
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, Main, nullptr, 0, 0);
        break;
    }
    return 1;
}

