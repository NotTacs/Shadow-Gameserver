#include "Player.h"
#include "Bots.h"
#include "SDK/SDK/FortniteGame_parameters.hpp"
#include "Inventory.h"

void ServerReadyToStartMatch(AFortPlayerControllerAthena* Controller) {

	std::cout << "ServerReadyToStartMatch" << std::endl;

	Controller->XPComponent->bRegisteredWithQuestManager = true;
	Controller->XPComponent->OnRep_bRegisteredWithQuestManager();

	std::cout << "Acolodes: " << Controller->XPComponent->PlayerAccolades.Num() << std::endl;

	if (!Controller->MatchReport) {
		std::cout << "Player has no Match Report" << std::endl;
	}

	if (Controller->IsPartyLeader()) {
		std::cout << "Party Leader !!" << std::endl;
	}

	auto PS = (AFortPlayerStateAthena*)Controller->PlayerState;


	return ServerReadyToStartMatch_OG(Controller);
}

void ServerAcknowledgePossesion(AFortPlayerController* Controller, APawn* Pawn) {

	Controller->AcknowledgedPawn = Pawn;

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
		if (!Controller->MyFortPawn) return;
		Controller->MyFortPawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Definition, ItemGuid);
	}
}

void ServerAttemptInventoryDrop(AFortPlayerController* Controller, const struct FGuid& ItemGuid, int32 Count, bool bTrash) {
	printf("ServerAttemptInventoryDrop");

	Inventory::RemoveItem((AFortPlayerControllerAthena*)Controller, ItemGuid, Count);

	return ServerAttemptInventoryDrop_OG(Controller, ItemGuid, Count, bTrash);
}

void ClientOnPawnDied(AFortPlayerControllerAthena* Controller, const struct FFortPlayerDeathReport& DeathReport) {
	AFortPlayerPawnAthena* KillerPawn = (AFortPlayerPawnAthena*)DeathReport.KillerPawn;
	AFortPlayerStateAthena* KillerPlayerState = (AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
	AFortPlayerStateAthena* DeadPlayerState = (AFortPlayerStateAthena*)Controller->PlayerState;
	printf("ClientOnPawnDied");
	if (DeathReport.ServerTimeForRespawn == 0 || DeathReport.ServerTimeForResurrect == 0) {
		printf("Player Will Not Respawn");
		if (KillerPawn && KillerPlayerState && Controller && !DeadPlayerState->DeathInfo.bDBNO) {
			for (UFortWorldItem* ItemInstance : Controller->WorldInventory->Inventory.ItemInstances) {
				Inventory::SpawnPickup(ItemInstance->ItemEntry.ItemDefinition, DeadPlayerState->DeathInfo.DeathLocation, ItemInstance->ItemEntry.Count, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination);

			}
		}
	}
	else {
		if (KillerPawn && KillerPlayerState && Controller && !DeadPlayerState->DeathInfo.bDBNO) {
			for (UFortWorldItem* ItemInstance : Controller->WorldInventory->Inventory.ItemInstances) {
				Inventory::SpawnPickup(ItemInstance->ItemEntry.ItemDefinition, DeadPlayerState->DeathInfo.DeathLocation, ItemInstance->ItemEntry.Count, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination);

			}
		}
	}

	return ClientOnPawnDied_OG(Controller, DeathReport);
}

void OnDamageServer(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext) {
	if (!InstigatedBy || !Object) return OnDamageServer_OG(Object, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
	if (Object->IsA(ABuildingSMActor::StaticClass())) {
		if (InstigatedBy->IsA(AFortPlayerControllerAthena::StaticClass()) && !Object->bIsPlayerBuildable) {
			AFortPlayerControllerAthena* FortController = (AFortPlayerControllerAthena*)InstigatedBy;
			UFortWeaponMeleeItemDefinition* PickaxeDef = FortController->CosmeticLoadoutPC.Pickaxe->WeaponDefinition;
			auto A = (ABuildingSMActor*)Object;
			if (FortController && FortController->MyFortPawn->CurrentWeapon && FortController->MyFortPawn->CurrentWeapon->WeaponData == PickaxeDef) {
				float What = FindCurveTable(StaticFindObject<UCurveTable>("/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates"), A->BuildingResourceAmountOverride.RowName);
				float Vhat = A->GetMaxHealth() / Damage;

				float sas = round(What / Vhat);

				FortController->ClientReportDamagedResourceBuilding(A, A->ResourceType, sas, false, Damage == 100.0f);
				Inventory::GiveWorldItem(FortController, UFortKismetLibrary::K2_GetResourceItemDefinition(A->ResourceType), sas, 0, true);
			}
		}
	}
	

	return OnDamageServer_OG(Object, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
}

void ServerCreateBuildingActor(AFortPlayerControllerAthena* Controller, const struct FCreateBuildingActorData& CreateBuildingData) {

	if (Controller->BroadcastRemoteClientInfo->RemoteBuildableClass && Controller) {
		FTransform Transform{};
		Transform.Translation = CreateBuildingData.BuildLoc;
		Transform.Rotation = ConvertRotToQuat(CreateBuildingData.BuildRot);
		Transform.Scale3D = FVector(1, 1, 1);
		ABuildingSMActor* Building = SpawnActor<ABuildingSMActor>(Transform, Controller, Controller->BroadcastRemoteClientInfo->RemoteBuildableClass);
		Building->bIsPlayerBuildable = true;
		Building->InitializeKismetSpawnedBuildingActor(Building, Controller, true);
		Building->TeamIndex = ((AFortPlayerStateAthena*)Controller->PlayerState)->TeamIndex;
		Building->Team = EFortTeam(Building->TeamIndex);

		Inventory::RemoveItem(Controller,
			Inventory::GetGuid(Controller, UFortKismetLibrary::K2_GetResourceItemDefinition(Building->ResourceType)),
			10);
	}

	return ServerCreateBuildingActor_OG(Controller, CreateBuildingData);
}

void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* Controller, ABuildingSMActor* BuildingActorToEdit) {
	if (!Controller) {
		return;
	}
	if (!BuildingActorToEdit) {
		printf("No building Actor");
		return;
	}
	FGuid Guid;
	UFortItemDefinition* ItemDefinition = nullptr;
	FFortItemEntry* EditToolEntry = nullptr;

	for (FFortItemEntry Entry : Controller->WorldInventory->Inventory.ReplicatedEntries) {
		if (Entry.ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass())) {
			EditToolEntry = &Entry;
			break;
		}
	}

	ItemDefinition = EditToolEntry->ItemDefinition;

	if (BuildingActorToEdit) {
		((AFortWeap_EditingTool*)ItemDefinition)->EditActor = BuildingActorToEdit;
		((AFortWeap_EditingTool*)ItemDefinition)->OnRep_EditActor();
	}

	Controller->ServerExecuteInventoryItem(EditToolEntry->ItemGuid);
}

ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor* Actor, unsigned int a2, UObject* a3, unsigned int a4, int a5, bool, AFortPlayerControllerAthena*) = decltype(ReplaceBuildingActor)(ImageBase + 0x1b951b0);

void ServerEditBuildingActor(AFortPlayerControllerAthena* Controller, class ABuildingSMActor* BuildingActorToEdit, TSubclassOf<class ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored) {
	printf("ServerEditBuildingActor");
	
	if (!BuildingActorToEdit || !NewBuildingClass || !Controller) return;

	BuildingActorToEdit->SetNetDormancy(ENetDormancy::DORM_DormantAll);
	BuildingActorToEdit->EditingPlayer = nullptr;

	ABuildingSMActor* BuildingActor = ReplaceBuildingActor(BuildingActorToEdit, 1, NewBuildingClass, BuildingActorToEdit->CurrentBuildingLevel, RotationIterations, bMirrored, Controller);

	if (BuildingActor)
		BuildingActor->bPlayerPlaced = true;
}

void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* ActorToStopEditing) {
	if (!PC || !PC->MyFortPawn || !ActorToStopEditing)
		return;
	ActorToStopEditing->SetNetDormancy(ENetDormancy::DORM_DormantAll);
	ActorToStopEditing->EditingPlayer = nullptr;

	AFortWeap_EditingTool* EditTool = (AFortWeap_EditingTool*)PC->MyFortPawn->CurrentWeapon;

	if (!EditTool)
		return;
	/*
	EditTool->EditActor = nullptr;
	EditTool->OnRep_EditActor(); */
}