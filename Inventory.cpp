#include "Inventory.h"

UFortWorldItem* Inventory::GiveWorldItem(AFortPlayerControllerAthena* PC, UFortItemDefinition* Definition, int Count, int Level, bool Stack, bool Pickup) {


	UFortWorldItem* WorldItem = (UFortWorldItem*)Definition->CreateTemporaryItemInstanceBP(Count, Level);
	WorldItem->SetOwningControllerForTemporaryItem(PC);
	WorldItem->OwnerInventory = PC->WorldInventory;
	WorldItem->ItemEntry.ItemDefinition = Definition;
	WorldItem->ItemEntry.Count = Count;
	WorldItem->ItemEntry.Level = Level;
	WorldItem->ItemEntry.LoadedAmmo = 0;

	if (Pickup) {
		FFortItemEntryStateValue State{};
		State.IntValue = 1;
		State.StateType = EFortItemEntryState::ShouldShowItemToast;
		WorldItem->ItemEntry.StateValues.Add(State);
	}


	if (Stack) {
		for (int i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++) {
			auto Entry = PC->WorldInventory->Inventory.ReplicatedEntries[i];
			if (Entry.ItemDefinition == Definition) {
				WorldItem->ItemEntry.Count += Entry.Count;
				PC->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
			}
		}
		for (int Piddy = 0; Piddy < PC->WorldInventory->Inventory.ItemInstances.Num(); Piddy++) {
			auto Rizz = PC->WorldInventory->Inventory.ItemInstances[Piddy];
			if (Rizz->ItemEntry.ItemDefinition == Definition) {
				PC->WorldInventory->Inventory.ItemInstances.Remove(Piddy);
			}
		}
	}

	
	PC->WorldInventory->Inventory.ItemInstances.Add(WorldItem);
	PC->WorldInventory->Inventory.ReplicatedEntries.Add(WorldItem->ItemEntry);
	PC->WorldInventory->bRequiresLocalUpdate = true;
	PC->WorldInventory->HandleInventoryLocalUpdate();
	PC->WorldInventory->Inventory.MarkItemDirty(WorldItem->ItemEntry);
	PC->WorldInventory->Inventory.MarkArrayDirty();

	return WorldItem;
}

void Inventory::RemoveItem(AFortPlayerControllerAthena* PC, FGuid ItemGuid, int Count, bool bSpawnPickup) {
	UFortWorldItem* WorldItem = nullptr;
	UFortItemDefinition* ItemDefinition = nullptr;
	bool bWipeInstance = false;
	bool ModifyEntry = false;
	int32 C;

	FFortItemEntry* RE = nullptr;

	for (UFortWorldItem* ItemInstance : PC->WorldInventory->Inventory.ItemInstances) {
		if (ItemInstance->ItemEntry.ItemGuid == ItemGuid) {
			WorldItem = ItemInstance;
			ItemDefinition = WorldItem->ItemEntry.ItemDefinition;
			break;
		}
	}

	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++) {
		FFortItemEntry Entry = PC->WorldInventory->Inventory.ReplicatedEntries[i];
		if (Entry.ItemDefinition == ItemDefinition) {
			C = Entry.Count -= Count;
			RE = &Entry;

			if (Entry.Count <= 0) {
				PC->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
				bWipeInstance = true;
				break;
			} else {
				PC->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
			}

			bWipeInstance = true;
			ModifyEntry = true;
			break;
		}
	}

	if (bWipeInstance && RE) {
		for (int c = 0; c < PC->WorldInventory->Inventory.ItemInstances.Num(); c++) {
			UFortWorldItem* ItemInstance = PC->WorldInventory->Inventory.ItemInstances[c];
			if (ItemInstance->ItemEntry.ItemGuid == RE->ItemGuid) {
				PC->WorldInventory->Inventory.ItemInstances.Remove(c);
				if (bSpawnPickup) {
					SpawnPickup(ItemInstance->ItemEntry.ItemDefinition,
						PC->MyFortPawn->K2_GetActorLocation(),
						C == 0 ? ItemInstance->ItemEntry.Count : C,
						EFortPickupSourceTypeFlag::Player,
						EFortPickupSpawnSource::Unset,
						PC->MyFortPawn);
				}
				break;
			}
		}
	}


	FFortItemEntry NewEntry;

	if (ModifyEntry) {
		UFortWorldItem* W = (UFortWorldItem*)ItemDefinition->CreateTemporaryInstanceFromExistingItemBP(WorldItem, C, 0);

		NewEntry = W->ItemEntry;
		PC->WorldInventory->Inventory.ReplicatedEntries.Add(NewEntry);
		PC->WorldInventory->Inventory.ItemInstances.Add(W);
	}


	PC->WorldInventory->bRequiresLocalUpdate = true;
	PC->WorldInventory->HandleInventoryLocalUpdate();
	if (ModifyEntry) {
		PC->WorldInventory->Inventory.MarkItemDirty(NewEntry);
	}
	else {
		//PC->WorldInventory->Inventory.MarkItemDirty(*RE);
		PC->WorldInventory->Inventory.MarkArrayDirty();
	}
}

AFortPickupAthena* Inventory::SpawnPickup(UFortItemDefinition* Definition, FVector Loc, int Count, EFortPickupSourceTypeFlag SourceTypeFlag, EFortPickupSpawnSource SpawnSource, AFortPlayerPawn* Pawn) {
	
	if (!Definition) return nullptr;
	FTransform Transform{};
	Transform.Translation = Loc;
	Transform.Rotation = {};
	Transform.Scale3D = FVector{ 1,1,1 };
	AFortPickupAthena* NewPickup = SpawnActor<AFortPickupAthena>(Transform);
	NewPickup->bRandomRotation = true;
	NewPickup->PrimaryPickupItemEntry.ItemDefinition = Definition;
	NewPickup->PrimaryPickupItemEntry.Count = Count;
	NewPickup->PrimaryPickupItemEntry.LoadedAmmo = 0;
	NewPickup->OnRep_PrimaryPickupItemEntry();
	NewPickup->PawnWhoDroppedPickup = Pawn;

	NewPickup->TossPickup(Loc, Pawn, -1, true, false, SourceTypeFlag, SpawnSource);

	return NewPickup;
}

FGuid Inventory::GetGuid(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def) {
	for (UFortWorldItem* Item : PC->WorldInventory->Inventory.ItemInstances) {
		if (Item->ItemEntry.ItemDefinition == Def) {
			return Item->ItemEntry.ItemGuid;
		}
		else continue;
	}
}

bool Inventory::ItemIsInInventory(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def) {
	for (UFortWorldItem* Raax : PC->WorldInventory->Inventory.ItemInstances) {
		if (Raax->ItemEntry.ItemDefinition == Def) {
			return true;
		}
	}

	return false;
}

FFortItemEntry Inventory::GetEntry(AFortPlayerControllerAthena* PC, FGuid Guid) {
	for (FFortItemEntry Entry : PC->WorldInventory->Inventory.ReplicatedEntries) {
		if (Entry.ItemGuid == Guid) return Entry;
	}
}