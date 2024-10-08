#include "Player.h"
#include "Bots.h"
#include "SDK/SDK/FortniteGame_parameters.hpp"
#include "Inventory.h"
#include "Looting.h"

void ServerReadyToStartMatch(AFortPlayerControllerAthena* Controller) {
	auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

	auto PlayerState = (AFortPlayerStateAthena*)Controller->PlayerState;
	auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;

	FGameMemberInfo MemberInfo{};
	MemberInfo.MostRecentArrayReplicationKey = -1;
	MemberInfo.ReplicationID = -1;
	MemberInfo.ReplicationKey = -1;
	MemberInfo.MemberUniqueId = PlayerState->UniqueId;
	MemberInfo.TeamIndex = PlayerState->TeamIndex;
	MemberInfo.SquadId = PlayerState->SquadId;

	GameState->GameMemberInfoArray.Members.Add(MemberInfo);
	GameState->GameMemberInfoArray.MarkItemDirty(MemberInfo);


	std::cout << GameMode->NumTeams << std::endl;

	Controller->XPComponent->bRegisteredWithQuestManager = true;
	Controller->XPComponent->OnRep_bRegisteredWithQuestManager();

	PlayerState->SeasonLevelUIDisplay = Controller->XPComponent->CurrentLevel;
	PlayerState->OnRep_SeasonLevelUIDisplay();

	return ServerReadyToStartMatch_OG(Controller);
}

void ServerAcknowledgePossesion(AFortPlayerController* Controller, APawn* Pawn) {

	Controller->AcknowledgedPawn = Pawn;

	static auto Set = StaticLoadObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer");
	Abilities::GiveAbilitySet(Controller->MyFortPawn, Set);

	AFortPlayerState* PlayerState = reinterpret_cast<AFortPlayerState*>(Controller->PlayerState);
	PlayerState->HeroType = Controller->CosmeticLoadoutPC.Character->HeroDefinition;
	UFortKismetLibrary::GetDefaultObj()->UpdatePlayerCustomCharacterPartsVisualization(PlayerState);
}

void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Component, FRotator ClientRotation) {
	std::cout << Component->GetOwner()->Class->GetName() << std::endl;
	AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
	AFortPlayerControllerAthena* PlayerController = reinterpret_cast<AFortPlayerControllerAthena*>(Component->GetOwner());
	if (PlayerController->IsInAircraft()) {
		UWorld::GetWorld()->AuthorityGameMode->RestartPlayer(PlayerController);
		
	}
}

AFortWeapon* ServerExecuteInventoryItem(AFortPlayerController* Controller, FGuid ItemGuid) {
	if (Controller->IsInAircraft()) return nullptr;

	UFortItemDefinition* Definition = nullptr;

	for (UFortWorldItem* WorldItem : Controller->WorldInventory->Inventory.ItemInstances) {
		if (WorldItem->ItemEntry.ItemGuid == ItemGuid) Definition = WorldItem->ItemEntry.ItemDefinition;
	}

	if (!Definition) return nullptr;

	if (!Controller) return nullptr;
	
	if (!Controller->MyFortPawn) return nullptr;

	return Controller->MyFortPawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Definition, ItemGuid);
}

void ServerClientIsReadyToRespawn(AFortPlayerControllerAthena* Controller) {
	auto GameMode = (AFortGameModeAthena*)GetWorld()->AuthorityGameMode;
	auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;
	auto PlayerState = (AFortPlayerStateAthena*)Controller->PlayerState;

	printf("ServerClientIsReadyToRespawn");
	
	if (PlayerState->RespawnData.bServerIsReady && PlayerState->RespawnData.bRespawnDataAvailable) {
		PlayerState->RespawnData.bClientIsReady = true;
		
		printf("RestartingPlayer");
		GameMode->RestartPlayer(Controller);
	}

	return ServerClientIsReadyToRespawn_OG(Controller);
}

void ServerAttemptInventoryDrop(AFortPlayerController* Controller, const struct FGuid& ItemGuid, int32 Count, bool bTrash) {

	std::cout << "Count: " << Count << std::endl;

	Inventory::RemoveItem((AFortPlayerControllerAthena*)Controller, ItemGuid, Count, true);

	return ServerAttemptInventoryDrop_OG(Controller, ItemGuid, Count, bTrash);
}

void ServerAttemptInteract(UFortControllerComponent_Interaction* Component, class AActor* ReceivingActor, class UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, class UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId) {
	auto PlayerController = (AFortPlayerControllerAthena*)Component->GetOwner();

	std::cout << PlayerController->Class->GetFullName() << std::endl;

	if (ReceivingActor->IsA(ABuildingContainer::StaticClass())) {
		ABuildingContainer* Container = (ABuildingContainer*)ReceivingActor;
		std::cout << "Container: " << Container->GetName() << std::endl;
		std::cout << "Mesh: " << Container->StaticMeshComponent->StaticMesh->GetName() << std::endl;
		std::cout << "ContainerSearchLootTierGroup: " << Container->SearchLootTierGroup.ToString() << std::endl;

		Container->bAlreadySearched = true;
		Container->OnRep_bAlreadySearched();
		Container->SearchBounceData.SearchAnimationCount++;
		Container->BounceContainer();
	}
	if (ReceivingActor->GetName().contains("Quest")) {
		std::cout << ReceivingActor->GetName() << std::endl;
	}

	return ServerAttemptInteract_OG(Component, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);
}

static void (*RemoveFromAlivePlayers)(AFortGameMode* GameMode, AFortPlayerController* PC, AFortPlayerState* State, AFortPawn* Pawn, UFortWeaponItemDefinition* Definition, EDeathCause Cause, char) = decltype(RemoveFromAlivePlayers)(ImageBase + 0x18ECBB0);

void ClientOnPawnDied(AFortPlayerControllerAthena* Controller, const struct FFortPlayerDeathReport& DeathReport) {
	if (!Controller) return ClientOnPawnDied_OG(Controller, DeathReport);
	AFortPlayerPawnAthena* KillerPawn = (AFortPlayerPawnAthena*)DeathReport.KillerPawn;
	UFortWeaponItemDefinition* Weapon = DeathReport.KillerWeapon;
	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
	AFortPlayerControllerAthena* KillerController =  (DeathReport.KillerPawn) ? (AFortPlayerControllerAthena*)DeathReport.KillerPawn->Controller : nullptr;
	AFortPlayerStateAthena* DeadState = (AFortPlayerStateAthena*)Controller->PlayerState;
	AFortPlayerPawnAthena* DeadPawn = (AFortPlayerPawnAthena*)Controller->MyFortPawn;
	AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

	DeadState->DeathInfo.bDBNO = Controller->MyFortPawn->bWasDBNOOnDeath;
	DeadState->DeathInfo.DeathLocation = Controller->MyFortPawn->K2_GetActorLocation();
	DeadState->DeathInfo.DeathCause = DeadState->ToDeathCause(DeathReport.Tags, Controller->MyFortPawn->bWasDBNOOnDeath);
	DeadState->DeathInfo.DeathTags = DeathReport.Tags;
	DeadState->DeathInfo.Distance = KillerController ? Controller->GetDistanceTo(KillerController->MyFortPawn) : 0;
	DeadState->DeathInfo.Downer = KillerPawn;
	DeadState->DeathInfo.FinisherOrDowner = KillerPawn;
	DeadState->DeathInfo.bInitialized = true;
	DeadState->OnRep_DeathInfo();

	for (UFortWorldItem* Item : Controller->WorldInventory->Inventory.ItemInstances) {
		if (Item->ItemEntry.ItemDefinition->IsA(UFortWeaponRangedItemDefinition::StaticClass())
			|| Item->ItemEntry.ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass())
			|| Item->ItemEntry.ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()))
		{
			Inventory::SpawnPickup(Item->ItemEntry.ItemDefinition, DeadState->DeathInfo.DeathLocation, Item->ItemEntry.Count, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, KillerPawn);
		}
	}

	for (int i = 0; i < Controller->WorldInventory->Inventory.ItemInstances.Num(); i++) {
		Controller->WorldInventory->Inventory.ItemInstances.Remove(i);
		Controller->WorldInventory->bRequiresLocalUpdate = true;
		Controller->WorldInventory->HandleInventoryLocalUpdate();
		Controller->WorldInventory->Inventory.MarkArrayDirty();
	}

	for (int i = 0; i < Controller->WorldInventory->Inventory.ReplicatedEntries.Num(); i++) {
		Controller->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
		Controller->WorldInventory->bRequiresLocalUpdate = true;
		Controller->WorldInventory->HandleInventoryLocalUpdate();
		Controller->WorldInventory->Inventory.MarkArrayDirty();
	}

	std::cout << DeathReport.ServerTimeForRespawn << std::endl;
	std::cout << DeathReport.ServerTimeForResurrect << std::endl;

	if (DeathReport.ServerTimeForRespawn != 0 || DeathReport.ServerTimeForResurrect != 0) {
	}

	printf("ClientOnPawnDied");

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
				Inventory::GiveWorldItem(FortController, UFortKismetLibrary::K2_GetResourceItemDefinition(A->ResourceType), sas, 0, true, false);
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

void ServerHandlePickup(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, float InFlyTime, const struct FVector& InStartDirection, bool bPlayPickupSound) {
	InFlyTime = 0.4;

	Pickup->PickupLocationData.bPlayPickupSound = bPlayPickupSound;
	Pickup->PickupLocationData.ItemOwner = Pawn;
	Pickup->PickupLocationData.StartDirection = FVector_NetQuantizeNormal(InStartDirection);
	Pickup->PickupLocationData.PickupTarget = Pawn;
	Pickup->PickupLocationData.FlyTime = InFlyTime;
	Pickup->OnRep_PickupLocationData();
	Pickup->OnRep_PickupOwnerData();
	Pickup->bPickedUp = true;
	Pickup->OnRep_bPickedUp();
}

char CompletePickupAnimation(AFortPickup* Pickup) {
	auto PC = (AFortPlayerControllerAthena*)Pickup->PickupLocationData.PickupTarget->Controller;
	if (!PC) return CompletePickupAnimation_OG(Pickup);
	if (PC->SwappingItemDefinition) {
		Inventory::SpawnPickup(PC->SwappingItemDefinition, PC->Pawn->K2_GetActorLocation(), PC->SwappingItemDefinition->DropCount, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PC->MyFortPawn);
		PC->SwappingItemDefinition = nullptr;
	}
	else {
		int Items = 0;
		for (FFortItemEntry Entry : PC->WorldInventory->Inventory.ReplicatedEntries) {
			if (Inventory::IsPrimaryQuickbar(Entry.ItemDefinition)) Items++;
		}
		if (Items >= 5 && Inventory::IsPrimaryQuickbar(Pickup->PrimaryPickupItemEntry.ItemDefinition) && Inventory::IsPrimaryQuickbar(PC->MyFortPawn->CurrentWeapon->WeaponData)) {

			FFortItemEntry ItemEntry = Inventory::GetEntry(PC, PC->MyFortPawn->CurrentWeapon->ItemEntryGuid);
			PC->ServerAttemptInventoryDrop(ItemEntry.ItemGuid, ItemEntry.Count, false);
		}
		else {
		}

		if (Inventory::ItemIsInInventory(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition) && Pickup->PrimaryPickupItemEntry.ItemDefinition->IsStackable() || Pickup->PrimaryPickupItemEntry.ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) && Pickup->PrimaryPickupItemEntry.Count == Pickup->PrimaryPickupItemEntry.ItemDefinition->MaxStackSize.Value) {
			Inventory::GiveWorldItem(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, 0, true, true);
		}
		else {
			Inventory::GiveWorldItem(PC, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, 0, false, true);
			Items++;
		}
	}
	return CompletePickupAnimation_OG(Pickup);
}

void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* Controller, ABuildingSMActor* BuildingActorToEdit) {
	if (!Controller) {
		return;
	}
	if (!BuildingActorToEdit) {
		printf("No building Actor");
		return;
	}

	FFortItemEntry EditToolEntry;

	static UFortEditToolItemDefinition* EditTool = UObject::FindObject<UFortEditToolItemDefinition>("FortEditToolItemDefinition EditTool.EditTool");

	for (const FFortItemEntry& Entry : Controller->WorldInventory->Inventory.ReplicatedEntries) {
		if (Entry.ItemDefinition == EditTool) {
			EditToolEntry = Entry;
			break;
		}
	}


	BuildingActorToEdit->EditingPlayer = (AFortPlayerStateAthena*)Controller->PlayerState;
	BuildingActorToEdit->OnRep_EditingPlayer();

	AFortWeap_EditingTool* EditingTool = (AFortWeap_EditingTool*)ServerExecuteInventoryItem(Controller, EditToolEntry.ItemGuid);

	if (EditingTool) {
		EditingTool->EditActor = BuildingActorToEdit;
		EditingTool->OnRep_EditActor();
	}
}

ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor* Actor, unsigned int a2, UObject* a3, unsigned int a4, int a5, bool, AFortPlayerControllerAthena*) = decltype(ReplaceBuildingActor)(ImageBase + 0x1b951b0);

void ServerEditBuildingActor(AFortPlayerControllerAthena* Controller, class ABuildingSMActor* BuildingActorToEdit, TSubclassOf<class ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored) {
	
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