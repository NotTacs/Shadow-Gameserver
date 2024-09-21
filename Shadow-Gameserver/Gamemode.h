#pragma once
#include "framework.h"

bool ReadyToStartMatch_Hook(AAthena_GameMode_C* GM);

APawn* SpawnDefaultPawnFor_Hook(AFortGameModeAthena* GM, AFortPlayerControllerAthena* Controller, AActor* StartSpot);