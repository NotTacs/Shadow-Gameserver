#include "Inventory.h"

UFortWorldItem* Inventory::GiveWorldItem(AFortPlayerControllerAthena* PC, UFortItemDefinition* Definition, int Count, int Level) {
	UFortWorldItem* WorldItem = (UFortWorldItem*)Definition->CreateTemporaryItemInstanceBP(Count, Level);
	WorldItem->SetOwningControllerForTemporaryItem(PC);
	WorldItem->OwnerInventory = PC->WorldInventory;
	WorldItem->ItemEntry.ItemDefinition = Definition;
	WorldItem->ItemEntry.Count = Count;
	WorldItem->ItemEntry.Level = Level;
	WorldItem->ItemEntry.LoadedAmmo = 0;
	
	PC->WorldInventory->Inventory.ItemInstances.Add(WorldItem);
	PC->WorldInventory->Inventory.ReplicatedEntries.Add(WorldItem->ItemEntry);
	PC->WorldInventory->bRequiresLocalUpdate = true;
	PC->WorldInventory->HandleInventoryLocalUpdate();
	PC->WorldInventory->Inventory.MarkItemDirty(WorldItem->ItemEntry);

	std::cout << PC->WorldInventory->Inventory.ItemInstances.Num() << std::endl;

	return WorldItem;
}