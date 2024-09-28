#pragma once
#include "framework.h"

namespace GameMode
{
	bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode);
	APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);

	inline void (*HandleStartingNewPlayer_OG)(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PC);
	void HandleStartingNewPlayer(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PC);
}