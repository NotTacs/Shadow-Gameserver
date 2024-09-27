#pragma once
#include "framework.h"
#include <fstream>

void DumpAllPatrolPathObjects();

AFortPlayerPawnAthena* SpawnBot(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData);