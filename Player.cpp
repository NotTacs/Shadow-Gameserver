#include "Player.h"
#include "Bots.h"
#include "SDK/SDK/FortniteGame_parameters.hpp"
#include "Inventory.h"

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


void OnDamageServer(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext) {
	printf("OnDamageServer");

	std::cout << "Class: " << Object->Class->GetFullName() << std::endl; 

	std::cout << "Class2: " << InstigatedBy->Class->GetFullName() << std::endl;

	if (InstigatedBy->IsA(AFortPlayerControllerAthena::StaticClass()) && !Object->bIsPlayerBuildable) {
		AFortPlayerControllerAthena* FortController = (AFortPlayerControllerAthena*)InstigatedBy;
		UFortWeaponMeleeItemDefinition* PickaxeDef = FortController->CosmeticLoadoutPC.Pickaxe->WeaponDefinition;
		auto A = (ABuildingSMActor*)Object;
		if (FortController && FortController->MyFortPawn->CurrentWeapon && FortController->MyFortPawn->CurrentWeapon->WeaponData == PickaxeDef) {
			float What = FindCurveTable(StaticFindObject<UCurveTable>("/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates"), A->BuildingResourceAmountOverride.RowName);
			std::cout << "What: " << What << std::endl;
			float Vhat = A->GetMaxHealth() / Damage;

			float sas = round(What / Vhat);

			std::cout << "sas: " << sas << std::endl;

			FortController->ClientReportDamagedResourceBuilding(A, A->ResourceType, sas, false, Damage == 100.0f);
			printf("test");
			Inventory::GiveWorldItem(FortController, UFortKismetLibrary::K2_GetResourceItemDefinition(A->ResourceType), sas, 0, true);
		}
	}

	return OnDamageServer_OG(Object, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
}