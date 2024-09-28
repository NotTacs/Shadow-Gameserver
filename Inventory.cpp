#include "Inventory.h"

UFortWorldItem* Inventory::GiveWorldItem(AFortPlayerControllerAthena* PC, UFortItemDefinition* Definition, int Count, int Level, bool Stack) {
	UFortWorldItem* WorldItem = (UFortWorldItem*)Definition->CreateTemporaryItemInstanceBP(Count, Level);
	WorldItem->SetOwningControllerForTemporaryItem(PC);
	WorldItem->OwnerInventory = PC->WorldInventory;
	WorldItem->ItemEntry.ItemDefinition = Definition;
	std::cout << "count: " << Count << std::endl;
	WorldItem->ItemEntry.Count = Count;
	WorldItem->ItemEntry.Level = Level;
	WorldItem->ItemEntry.LoadedAmmo = 0;

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