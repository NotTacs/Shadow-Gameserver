#pragma once
#include "framework.h"

namespace GameMode
{
	bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode);
	APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot);
}