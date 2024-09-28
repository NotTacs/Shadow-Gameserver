#pragma once
#include "framework.h"
#include <fstream>

void DumpAllPatrolPathObjects();

inline AFortPlayerPawnAthena* (*SpawnBot_OG)(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData);
AFortPlayerPawnAthena* SpawnBot(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData);