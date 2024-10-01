#include "Player.h"
#include "Bots.h"
#include "SDK/SDK/FortniteGame_parameters.hpp"
#include "Inventory.h"
#include "Looting.h"

void ServerReadyToStartMatch(AFortPlayerControllerAthena* Controller) {

	std::cout << "ServerReadyToStartMatch" << std::endl;
	auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

	static bool bFirst = false;
	if (!bFirst) {
		TArray<AActor*> Actors;
		TArray<AActor*> ActorsAgain;
		static UClass* Tiered_Athena_FloorLoot_Warmup = StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
		static UClass* Tiered_Athena_FloorLoot = StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), Tiered_Athena_FloorLoot_Warmup, &Actors);
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), Tiered_Athena_FloorLoot, &ActorsAgain);

		for (int i = 0; i < Actors.Num(); i++) {
			auto Container = (ABuildingContainer*)Actors[i];
			auto TierGroup = GetItems(Container->SearchLootTierGroup);
			for (auto& Item : TierGroup) {
				Inventory::SpawnPickup(Item.ItemDefinition, Container->K2_GetActorLocation(), Item.Count, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);

				auto T = (UFortWeaponRangedItemDefinition*)Item.ItemDefinition;

				if (((UFortWeaponRangedItemDefinition*)Item.ItemDefinition)->GetAmmoWorldItemDefinition_BP() && T->AmmoData.Get() != T) {
					auto AmmoDef = ((UFortWeaponRangedItemDefinition*)Item.ItemDefinition)->GetAmmoWorldItemDefinition_BP();

					Inventory::SpawnPickup(((UFortWeaponRangedItemDefinition*)Item.ItemDefinition)->GetAmmoWorldItemDefinition_BP(), Container->K2_GetActorLocation(), AmmoDef->DropCount, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);
				}
			}
			Container->K2_DestroyActor();
		}

		for (int i = 0; i < ActorsAgain.Num(); i++) {
			auto Container = (ABuildingContainer*)ActorsAgain[i];
			auto TierGroup = GetItems(Container->SearchLootTierGroup);
			for (auto& Item : TierGroup) {
				Inventory::SpawnPickup(Item.ItemDefinition, Container->K2_GetActorLocation(), Item.Count, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);
				auto T = (UFortWeaponRangedItemDefinition*)Item.ItemDefinition;

				if (((UFortWeaponRangedItemDefinition*)Item.ItemDefinition)->GetAmmoWorldItemDefinition_BP() && T->AmmoData.Get() != T) {
					auto AmmoDef = ((UFortWeaponRangedItemDefinition*)Item.ItemDefinition)->GetAmmoWorldItemDefinition_BP();
					
					Inventory::SpawnPickup(((UFortWeaponRangedItemDefinition*)Item.ItemDefinition)->GetAmmoWorldItemDefinition_BP(), Container->K2_GetActorLocation(), AmmoDef->DropCount, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);
				}
			}
			Container->K2_DestroyActor();
		}

		Actors.Free();
		ActorsAgain.Free();

		bFirst = true;
	}

	auto PlayerState = (AFortPlayerStateAthena*)Controller->PlayerState;
	auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;

	std::cout << "GameMemberArray: " << GameState->GameMemberInfoArray.Members.Num() << std::endl;

	FGameMemberInfo MemberInfo{};
	MemberInfo.MostRecentArrayReplicationKey = -1;
	MemberInfo.ReplicationID = -1;
	MemberInfo.ReplicationKey = -1;
	MemberInfo.MemberUniqueId = PlayerState->UniqueId;
	MemberInfo.TeamIndex = PlayerState->TeamIndex;
	MemberInfo.SquadId = PlayerState->SquadId;

	GameState->GameMemberInfoArray.Members.Add(MemberInfo);
	GameState->GameMemberInfoArray.MarkItemDirty(MemberInfo);

	std::cout << "GameMembersNow: " << GameState->GameMemberInfoArray.Members.Num() << std::endl;

	Sleep(2000);

	std::cout << GameMode->NumTeams << std::endl;

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

	std::cout << "Def: " << Definition->GetFullName() << "\n";

	Controller->MyFortPawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Definition, ItemGuid);
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

__int64 (*CantBuild)(UWorld*, UClass*, FVector, FRotator, bool, TArray<ABuildingSMActor*>*, char*) = decltype(CantBuild)(ImageBase + 0x1e57790);

void ServerCreateBuildingActor(AFortPlayerControllerAthena* Controller, const struct FCreateBuildingActorData& CreateBuildingData) {



	if (Controller->BroadcastRemoteClientInfo->RemoteBuildableClass && Controller) {
		TArray<ABuildingSMActor*> ActorsToRemove;
		char a7;
		if (!CantBuild(GetWorld(), Controller->BroadcastRemoteClientInfo->RemoteBuildableClass.Get(), CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &ActorsToRemove, &a7)) {
			FTransform Transform{};
			Transform.Translation = CreateBuildingData.BuildLoc;
			Transform.Rotation = ConvertRotToQuat(CreateBuildingData.BuildRot);
			Transform.Scale3D = FVector(1, 1, 1);
			std::cout << "BuildingClass: " << Controller->BroadcastRemoteClientInfo->RemoteBuildableClass.Get()->GetFullName() << "\n";
			ABuildingSMActor* Building = SpawnActor<ABuildingSMActor>(Transform, Controller, Controller->BroadcastRemoteClientInfo->RemoteBuildableClass.Get());
			Building->bPlayerPlaced = true;
			Building->SetMirrored(CreateBuildingData.bMirrored);
			Building->InitializeKismetSpawnedBuildingActor(Building, Controller, true);
			Building->TeamIndex = ((AFortPlayerStateAthena*)Controller->PlayerState)->TeamIndex;
			Building->Team = EFortTeam(Building->TeamIndex);

			for (int i = 0; i < ActorsToRemove.Num(); i++) {
				ActorsToRemove[i]->K2_DestroyActor();
			}
			ActorsToRemove.Free();

			Inventory::RemoveItem(Controller,
				Inventory::GetGuid(Controller, UFortKismetLibrary::K2_GetResourceItemDefinition(Building->ResourceType)),
				10);
		}
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

	std::cout << "BuildingActorToEdit: " << BuildingActorToEdit->GetFullName() << std::endl;

	AFortWeap_EditingTool* ItemDefinition = nullptr;
	FFortItemEntry EditToolEntry;

	static UFortEditToolItemDefinition* EditTool = UObject::FindObject<UFortEditToolItemDefinition>("FortEditToolItemDefinition EditTool.EditTool");

	for (const FFortItemEntry& Entry : Controller->WorldInventory->Inventory.ReplicatedEntries) {
		if (Entry.ItemDefinition == EditTool) {
			EditToolEntry = Entry;
			break;
		}
	}

	ItemDefinition = (AFortWeap_EditingTool*)EditToolEntry.ItemDefinition;

	BuildingActorToEdit->EditingPlayer = (AFortPlayerStateAthena*)Controller->PlayerState;
	BuildingActorToEdit->OnRep_EditingPlayer();

	printf("Rizz");

	Controller->ServerExecuteInventoryItem(EditToolEntry.ItemGuid);
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
	ActorToStopEditing->OnRep_EditingPlayer();

	AFortWeap_EditingTool* EditTool = (AFortWeap_EditingTool*)PC->MyFortPawn->CurrentWeapon;

	if (!EditTool)
		return;
	EditTool->bEditConfirmed = true;
	EditTool->EditActor = nullptr;
	EditTool->OnRep_EditActor();

}