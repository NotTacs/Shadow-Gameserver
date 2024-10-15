#include "GameMode.h"
#include "Inventory.h"
#include "Looting.h"
#include "Events.h"

bool GameMode::ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)GameMode->GameState;

	//std::cout << "ReadyToStartMatch" << std::endl;
	static bool bPlaylist = false;
	if (!bPlaylist)
	{
		static UFortPlaylistAthena* Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
		GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
		GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
		GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
		GameState->CurrentPlaylistInfo.MarkArrayDirty();
		GameState->CurrentPlaylistId = Playlist->PlaylistId;

		GameMode->CurrentPlaylistId = Playlist->PlaylistId;
		GameMode->CurrentPlaylistName = Playlist->PlaylistName;

		GameState->AirCraftBehavior = Playlist->AirCraftBehavior;
		GameState->CachedSafeZoneStartUp = Playlist->SafeZoneStartUp;

		float TimeSeconds = UGameplayStatics::GetTimeSeconds(GetWorld());
		float Duration = 120.f;

		GameState->bGameModeWillSkipAircraft = Playlist->bSkipAircraft;
		GameMode->bSpawnAllStuff = true;
		

		GameState->WarmupCountdownEndTime = TimeSeconds + Duration;
		GameMode->WarmupCountdownDuration = Duration;
		GameState->WarmupCountdownStartTime = TimeSeconds;
		GameMode->WarmupEarlyCountdownDuration = Duration;

		if (Playlist->bIsDefaultPlaylist) {
			GameMode->ServerBotManager = (UFortServerBotManagerAthena*)UGameplayStatics::SpawnObject(UFortServerBotManagerAthena::StaticClass(), GameMode);
			GameMode->ServerBotManagerClass = UFortServerBotManagerAthena::StaticClass();
			GameMode->ServerBotManager->CachedGameMode = GameMode;
			GameMode->ServerBotManager->CachedGameState = GameState;

			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFortAthenaMutator_Bots::StaticClass(), &Actors);
			for (AActor* Actor : Actors) {
				static bool bFirst = false;
				if (!bFirst) {
					GameMode->ServerBotManager->CachedBotMutator = ((AFortAthenaMutator_Bots*)Actor);
					bFirst = true;
				}
			}
			if (!GameMode->ServerBotManager->CachedBotMutator) {
				GameMode->ServerBotManager->CachedBotMutator = SpawnActor<AFortAthenaMutator_Bots>();
			}

			GameMode->AIDirector = SpawnActor<AFortAIDirector>();
			if (GameMode->AIDirector) {
				GameMode->AIDirector->Activate();
			}
			GameMode->AIGoalManager = SpawnActor<AFortAIGoalManager>();
		}

		if (Playlist->AISettings) {
			GameMode->AISettings = Playlist->AISettings;
			auto System = (UAthenaAISystem*)GetWorld()->AISystem;
			if (!System->BotManager) {
				System->BotManager = (UFortBotMissionManager*)UGameplayStatics::SpawnObject(UFortBotMissionManager::StaticClass(), System);
			}
			else {
				std::cout << System->BotManager->GetName() << std::endl;
			}

			if (!GameMode->AIDirector) {
				GameMode->AIDirector = SpawnActor<AFortAIDirector>();
				GameMode->AIDirector->Activate();
			}
			else {
				std::cout << GameMode->AIDirector->GetName() << std::endl;
			}
			std::cout << System->RegisteredAISpawners.Num() << std::endl;
			for (int i = 0; i < System->RegisteredAISpawners.Num();i++) {
				AActor* Spawner = System->RegisteredAISpawners[i];
				std::cout << "Spawner: " << Spawner->Class->GetName() << std::endl;
			}
		}

		

		bPlaylist = true;
	}

	if (!bCreative) {
		static bool bPlayerStartLcsLoaded = false;
		if (!bPlayerStartLcsLoaded) {
			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFortPlayerStartWarmup::StaticClass(), &Actors);
			int Num = Actors.Num();
			Actors.Free();
			if (Num == 0) {
				return false;
			}
			bPlayerStartLcsLoaded = true;
		}
	}

	if (!GameState->MapInfo) return false;

	if (GameState->Teams.Num() == 0) return false;

	static bool bListening = false;
	if (!bListening) {
		FName GameNetDriver = UKismetStringLibrary::Conv_StringToName(TEXT("GameNetDriver"));
		UNetDriver* (*CreateNetDriver)(UEngine*, UWorld*, FName) = decltype(CreateNetDriver)(ImageBase + 0x4573990);
		UNetDriver* Driver = CreateNetDriver(GEngine, GetWorld(), GameNetDriver);
		Driver->World = GetWorld();
		Driver->NetDriverName = GameNetDriver;

		FString Error;
		FURL URL;
		URL.Port = 7777;

		bool (*InitListen)(UNetDriver*, UWorld*, FURL&, bool, FString&) = decltype(InitListen)(ImageBase + 0xD44C40);
		void (*SetWorld)(UNetDriver*, UWorld*) = decltype(SetWorld)(ImageBase + 0x42C2B20);

		GameState->OnRep_CurrentPlaylistInfo();

		InitListen(Driver, GetWorld(), URL, false, Error);
		SetWorld(Driver, GetWorld());

		GetWorld()->NetDriver = Driver;
		for (int i = 0; i < GetWorld()->LevelCollections.Num(); i++)
		{
			GetWorld()->LevelCollections[i].NetDriver = Driver;
		}

		GameMode->bWorldIsReady = true;

		for (int i = 0; i < CurrentPlaylist()->AdditionalLevels.Num(); i++) {
			TSoftObjectPtr<UWorld> World = CurrentPlaylist()->AdditionalLevels[i];
			FString LevelName = UKismetStringLibrary::Conv_NameToString(World.ObjectID.AssetPathName);
			ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), LevelName, {}, {}, nullptr, FString());
			FAdditionalLevelStreamed NewLevel{World.ObjectID.AssetPathName,false};
			GameState->AdditionalPlaylistLevelsStreamed.Add(NewLevel);
		}
		for (int i = 0; i < CurrentPlaylist()->AdditionalLevelsServerOnly.Num(); i++) {
			TSoftObjectPtr<UWorld> World = CurrentPlaylist()->AdditionalLevelsServerOnly[i];
			FString LevelName = UKismetStringLibrary::Conv_NameToString(World.ObjectID.AssetPathName);
			ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), LevelName, {}, {}, nullptr, FString());
			FAdditionalLevelStreamed NewLevel{ World.ObjectID.AssetPathName,true };
			GameState->AdditionalPlaylistLevelsStreamed.Add(NewLevel);
		}
		GameState->OnFinishedShowingAdditionalPlaylistLevel();
		GameState->OnRep_AdditionalPlaylistLevelsStreamed(); 
		GameMode->HandleAllPlaylistLevelsVisible();

		GameState->OnRep_CurrentPlaylistId();
		SetConsoleTitleA("Listening");

		JerkyLoader::GetAllJerkyObjects();

		TArray<AActor*> WarmupActors;
		static UClass* WarmupClass = StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), WarmupClass, &WarmupActors);

		for (int i = 0; i < WarmupActors.Num(); i++) {
			auto Container = (ABuildingContainer*)WarmupActors[i];
			auto Entrys = GetItems(Container->SearchLootTierGroup);
			Container->K2_GetActorLocation().Z + 50;
			for (auto& item : Entrys) {
				Inventory::SpawnPickup(item.ItemDefinition, Container->K2_GetActorLocation(), item.Count, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);

				UFortWeaponRangedItemDefinition* Item = (UFortWeaponRangedItemDefinition*)item.ItemDefinition;

				if (Item->IsA(UFortWeaponRangedItemDefinition::StaticClass()) && Item->GetAmmoWorldItemDefinition_BP() && Item->AmmoData.Get() != Item) {
					auto AmmoDef = Item->GetAmmoWorldItemDefinition_BP();

					Inventory::SpawnPickup(AmmoDef, Container->K2_GetActorLocation(), AmmoDef->DropCount, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset);
				}
			}
			Container->K2_DestroyActor();
		}

		WarmupActors.Free();

		TArray<AActor*> FloorLootActors;
		static UClass* FloorlootClass = StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), FloorlootClass, &FloorLootActors);

		for (int i = 0; i < FloorLootActors.Num(); i++) {
			auto Container = (ABuildingContainer*)FloorLootActors[i];
			auto Entrys = GetItems(Container->SearchLootTierGroup);
			FVector Container2 = Container->K2_GetActorLocation() + FVector(0,0,50);

			for (auto& item : Entrys) {
				Inventory::SpawnPickup(item.ItemDefinition, Container2, item.Count, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);

				UFortWeaponRangedItemDefinition* Item = (UFortWeaponRangedItemDefinition*)item.ItemDefinition;

				if (Item->IsA(UFortWeaponRangedItemDefinition::StaticClass()) && Item->GetAmmoWorldItemDefinition_BP() && Item->AmmoData.Get() != Item) {
					auto AmmoDef = Item->GetAmmoWorldItemDefinition_BP();

					Inventory::SpawnPickup(AmmoDef, Container2, AmmoDef->DropCount, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset);
				}
			}
			Container->K2_DestroyActor();
		}
		FloorLootActors.Free();

		bListening = true;
	}

	bool bRet = GameMode->AlivePlayers.Num() > 0;

	if (!bRet) {
		float TimeSeconds = UGameplayStatics::GetTimeSeconds(GetWorld());
		float Duration = 120.f;

		GameState->WarmupCountdownEndTime = TimeSeconds + Duration;
		GameMode->WarmupCountdownDuration = Duration;
		GameState->WarmupCountdownStartTime = TimeSeconds;
		GameMode->WarmupEarlyCountdownDuration = Duration;
	}

	return bRet;
}

inline void CreatePair(std::vector<std::pair<UFortItemDefinition*, int>> Vector, UFortItemDefinition* Def, int Count) {
	std::pair<UFortItemDefinition*, int> Pair;
	Pair.first = Def;
	Pair.second = Count;
	Vector.push_back(Pair);
}

APawn* GameMode::SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot)
{

	

	auto GameState = (AFortGameStateAthena*)GameMode->GameState;
	for (int i = 0; i < GameMode->StartingItems.Num(); i++) {
		FItemAndCount Item = GameMode->StartingItems[i];
		Inventory::GiveWorldItem(NewPlayer, Item.Item, Item.Count, 0);
	}
	Inventory::GiveWorldItem(NewPlayer, NewPlayer->CosmeticLoadoutPC.Pickaxe->WeaponDefinition, 1, 0);

	std::ofstream Log("Weapons.Log");

	

	auto Pawn = (AFortPlayerPawnAthena*)GameMode->SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetTransform());



	if (bLateGame) {

		UFortItemDefinition* AmmoLightBullets = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
		UFortItemDefinition* AmmoHeavyBullets = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
		UFortItemDefinition* AmmoMediumBullets = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
		UFortItemDefinition* AmmoShells = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");

		Inventory::GiveWorldItem(NewPlayer, AmmoLightBullets, 500, 0, true, false);
		Inventory::GiveWorldItem(NewPlayer, AmmoHeavyBullets, 500, 0, true, false);
		Inventory::GiveWorldItem(NewPlayer, AmmoMediumBullets, 500, 0, true, false);
		Inventory::GiveWorldItem(NewPlayer, AmmoShells, 500, 0, true, false);


		for (int i = 0; i < UObject::GObjects->Num(); i++) {
			UObject* GObject = UObject::GObjects->GetByIndex(i);

			if (!GObject) continue;

			if (GObject->IsA(UFortItemDefinition::StaticClass()) && !GObject->IsDefaultObject()) {
				Log << "[" << ((UFortItemDefinition*)GObject)->DisplayName.ToString() << "] " << GObject->GetName() << std::endl;
			}
		}

		Log.close();
	}


	
	AActor* SpawnLocator = SpawnActor<ADefaultPawn>(StartSpot->GetTransform());
	UClass* Class = StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C");
	auto Test = (ABP_PlayerPawn_Athena_Phoebe_C*)GameMode->ServerBotManager->CachedBotMutator->SpawnBot(Class, SpawnLocator, SpawnLocator->K2_GetActorLocation(), FRotator(), false);

	SpawnLocator->K2_DestroyActor();

	auto Controller = (ABP_PhoebePlayerController_C*)Test->Controller;

	Controller->MoveToLocation(Pawn->K2_GetActorLocation(), 50000, false, false, true, true, Controller->DefaultNavigationFilterClass, true);

	UFortQuestManager* QuestManager = NewPlayer->GetQuestManager(GameMode->AssociatedSubGame);

	QuestManager->EnableQuestStateLogging();

	return Pawn;
}

//GD I Wasn't using this for a reason, it crashes on hooking


inline EFortTeam GetNextEmptyTeam(int MaxTeamCount, int MaxSquadSize) {
	auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;

	if (GameState) {
		auto Teams = GameState->Teams;
		for (int i = 3; i < Teams.Num(); i++) {
			auto Team = Teams[i];
			if (!Team.IsValid()) continue;
			if (Team.Num() <= MaxSquadSize && i <= (MaxTeamCount + 3)) return EFortTeam(i);
		}
	}

	return EFortTeam::Spectator;
}

inline EFortTeam GetLowestTeam(int MaxTeamCount, int MaxSquadSize) {
	auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;
	std::pair<int, int> LowestTeam = { 0,0 }; //TeamIndex,Num
	if (GameState) {
		auto Teams = GameState->Teams;
		for (int i = 3; i < Teams.Num(); i++) {
			auto Team = Teams[i];
			if (!Team.IsValid()) continue;
			if (Team.Num() < MaxSquadSize && i <= (MaxTeamCount + 3) && Team.Num() <= LowestTeam.second) {
				LowestTeam = { i,Team.Num() };
			}
		}
		return EFortTeam(LowestTeam.first);
	}

	return EFortTeam::Spectator;
}

EFortTeam GameMode::PickTeamHook(AFortGameModeAthena* GameMode, uint8_t Preferred, AFortPlayerControllerAthena* PC) {
	
	if (!GameMode || !PC) return EFortTeam::Spectator;

	UFortPlaylistAthena* Playlist = CurrentPlaylist();

	bool bSpreadTeams = (Playlist->bShouldSpreadTeams || Playlist->bIsLargeTeamGame);
	int MaxTeamCount = Playlist->MaxTeamCount;
	int MaxSquadSize = Playlist->MaxSquadSize;

	EFortTeam Ret = (bSpreadTeams ? (GetLowestTeam(MaxTeamCount, MaxSquadSize)) : (GetNextEmptyTeam(MaxTeamCount, MaxSquadSize)));

	/*

	if (PC->PlayerState) {
		AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
		PlayerState->SquadId = ((int)Ret - 3);
		if (GameMode->GameState) {
			AFortGameStateAthena* GameState = (AFortGameStateAthena*)GameMode->GameState;

			TWeakObjectPtr<AFortPlayerStateAthena> WeakPlayerState{ PlayerState->Index, UObject::GObjects->GetSNByIndex(PlayerState->Index) };

			GameState->Teams[(int)Ret].Add(WeakPlayerState);
			GameState->Squads[PlayerState->SquadId].Add(WeakPlayerState);

			FGameMemberInfo MemberInfo{ -1,-1,-1 ,PlayerState->SquadId,(int)Ret,0,0,PlayerState->UniqueId};
			GameState->GameMemberInfoArray.Members.Add(MemberInfo);
			GameState->GameMemberInfoArray.MarkArrayDirty();
		}
	}
	*/

	return Ret;
}

void GameMode::OnAircraftEnteredDropZone(AFortGameModeAthena* GM, AFortAthenaAircraft* Aircraft) {
	auto GameState = (AFortGameStateAthena*)GM->GameState;

	

	if (bLateGame) {
		
		static FVector AircraftLocation = ZoneLoc + FVector(0, 0, 10000);

		Aircraft->FlightEndTime = 100.f;
		Aircraft->FlightInfo.FlightSpeed = 1.0f;
		Aircraft->FlightInfo.FlightStartLocation = (FVector_NetQuantize100)AircraftLocation;
		Aircraft->FlightEndTime = 0.f;
		GameState->OnRep_Aircraft();

		GameState->GamePhase = EAthenaGamePhase::SafeZones;
		GameState->OnRep_GamePhase(EAthenaGamePhase::Aircraft);
		GameState->SafeZonesStartTime = 1;
	}

	return OnAircraftEnteredDropZone_OG(GM, Aircraft);
}