#pragma once
#include "framework.h"

namespace Inventory
{
	UFortWorldItem* GiveWorldItem(AFortPlayerControllerAthena* PC,UFortItemDefinition* Definition, int Count, int Level, bool Stack = false, bool Pickup = true);
	void RemoveItem(AFortPlayerControllerAthena* PC, FGuid ItemGuid, int Count, bool bSpawnPickup = false);
	AFortPickupAthena* SpawnPickup(UFortItemDefinition* Definition, FVector Loc, int Count, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn = nullptr);
	FGuid GetGuid(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def);
	bool ItemIsInInventory(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def);

	inline bool IsPrimaryQuickbar(UFortItemDefinition* Def)
	{
		return !Def->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) && !Def->IsA(UFortResourceItemDefinition::StaticClass()) && !Def->IsA(UFortAmmoItemDefinition::StaticClass()) && !Def->IsA(UFortTrapItemDefinition::StaticClass()) && !Def->IsA(UFortBuildingItemDefinition::StaticClass()) && !Def->IsA(UFortEditToolItemDefinition::StaticClass());
		//return Def->IsA<UFortConsumableItemDefinition>() || Def->IsA<UFortWeaponRangedItemDefinition>() || Def->IsA<UFortGadgetItemDefinition>();
	}

	FFortItemEntry GetEntry(AFortPlayerControllerAthena* PC, FGuid Guid);

	int GetClipSize(AFortPlayerControllerAthena* PC, UFortWeaponRangedItemDefinition* Def);
}