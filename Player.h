#pragma once
#include "framework.h"
#include "Abilities.h"

inline void (*ServerReadyToStartMatch_OG)(AFortPlayerController* Controller);
void ServerReadyToStartMatch(AFortPlayerController* Controller);

void ServerAcknowledgePossesion(AFortPlayerController* Controller, APawn* Pawn);

void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Component, FRotator ClientRotation);

void ServerExecuteInventoryItem(AFortPlayerController* Controller, FGuid ItemGuid);