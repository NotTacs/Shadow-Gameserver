#include "Player.h"
#include "Bots.h"

void ServerReadyToStartMatch(AFortPlayerController* Controller) {

	std::cout << "ServerReadyToStartMatch" << std::endl;

	return ServerReadyToStartMatch_OG(Controller);
}

void ServerAcknowledgePossesion(AFortPlayerController* Controller, APawn* Pawn) {

	Controller->AcknowledgedPawn = Pawn;

	DumpAllPatrolPathObjects();

	std::cout << "ServerAcknowledgePosession" << std::endl;
	static auto Set = StaticLoadObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer");
	Abilities::GiveAbilitySet(Controller->MyFortPawn, Set);

	AFortPlayerState* PlayerState = reinterpret_cast<AFortPlayerState*>(Controller->PlayerState);
	PlayerState->HeroType = Controller->CosmeticLoadoutPC.Character->HeroDefinition;
	UFortKismetLibrary::GetDefaultObj()->UpdatePlayerCustomCharacterPartsVisualization(PlayerState);
}

void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Component, FRotator ClientRotation) {
	std::cout << Component->GetOwner()->Class->GetName() << std::endl;
	AFortPlayerControllerAthena* PlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(Component->GetOwner());
	if (PlayerController->IsInAircraft()) {
		UWorld::GetWorld()->AuthorityGameMode->RestartPlayer(PlayerController);
	}
}

void ServerExecuteInventoryItem(AFortPlayerController* Controller, FGuid ItemGuid) {
	if (Controller->IsInAircraft()) return;

	UFortItemDefinition* Definition = nullptr;

	for (UFortWorldItem* WorldItem : Controller->WorldInventory->Inventory.ItemInstances) {
		if (WorldItem->ItemEntry.ItemGuid == ItemGuid) Definition = WorldItem->ItemEntry.ItemDefinition;
	}

	if (!Definition) return;

	if (Definition->IsA(UFortWeaponItemDefinition::StaticClass())) {
		Controller->MyFortPawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Definition, ItemGuid);
	}
}