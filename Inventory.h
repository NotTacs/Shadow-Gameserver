#pragma once
#include "framework.h"

namespace Inventory
{
	UFortWorldItem* GiveWorldItem(AFortPlayerControllerAthena* PC,UFortItemDefinition* Definition, int Count, int Level, bool Stack = false, bool Pickup = false);
	void RemoveItem(AFortPlayerControllerAthena* PC, FGuid ItemGuid, int Count);
	AFortPickupAthena* SpawnPickup(UFortItemDefinition* Definition, FVector Loc, int Count, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn = nullptr);
}