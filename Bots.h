#pragma once

#include "framework.h"
#include <fstream>

inline std::vector<AFortAthenaAIBotController*> Controllers;

void GiveItem(AFortAthenaAIBotController* Controller, UFortItemDefinition* Def, int count = 999, int Level = 0);

FGuid GetGuid(AFortAthenaAIBotController* Controller, UFortItemDefinition* Def);

inline AFortPlayerPawnAthena* (*SpawnBot_OG)(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData);
AFortPlayerPawnAthena* SpawnBot(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData);