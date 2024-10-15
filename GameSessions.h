#pragma once
#include "framework.h"

static bool bGameSessionsEnabled = true; //BackendSide When?

namespace GameSessions {
	inline TSubclassOf<AGameSession>(*GetGameSessionClass_OG)(AFortGameModeAthena*, __int64* SomethingIdk);
	__int64* GetGameSessionClass_Hook(AFortGameModeAthena*, __int64* SomethingIdk);

	void HookAll();
}