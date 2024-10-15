#include "GameSessions.h"

__int64* GameSessions::GetGameSessionClass_Hook(AFortGameModeAthena* GameMode, __int64* SomethingIdk) {
	std::cout << "GameSessionClass Called. Returning DedicatedAthena_GetPrivateStaticClass" << std::endl;
	GameMode->GameSessionClass = AFortGameSessionDedicatedAthena::StaticClass();
	*SomethingIdk = __int64(AFortGameSessionDedicatedAthena::StaticClass());
	return SomethingIdk;
}

void GameSessions::HookAll() {
	VFTHook(AFortGameModeAthena::GetDefaultObj()->VTable, 0xD3, GetGameSessionClass_Hook, nullptr); //This is only called by vft
}