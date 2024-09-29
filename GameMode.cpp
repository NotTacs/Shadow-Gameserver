#include "GameMode.h"
#include "Inventory.h"

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



		GameState->WarmupCountdownEndTime = TimeSeconds + Duration;
		GameMode->WarmupCountdownDuration = Duration;
		GameState->WarmupCountdownStartTime = TimeSeconds;
		GameMode->WarmupEarlyCountdownDuration = Duration;

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

		if (Playlist->AISettings) {
			GameMode->AISettings = Playlist->AISettings;
		}

		bPlaylist = true;
	}

	static bool bWait = false;
	if (!bWait) {
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFortPlayerStartWarmup::StaticClass(), &Actors);
		int Num = Actors.Num();
		Actors.Free();
		if (Num == 0) {
			return false;
		}
		bWait = true;
	}

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
		GameState->OnRep_CurrentPlaylistInfo();
		GameState->OnRep_CurrentPlaylistId();
		SetConsoleTitleA("Listening");

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

APawn* GameMode::SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot)
{
	printf("SpawnDefaultPawnFor");
	for (int i = 0; i < GameMode->StartingItems.Num(); i++) {
		FItemAndCount Item = GameMode->StartingItems[i];
		std::cout << "Item: " << Item.Item->GetFullName() << std::endl;
		Inventory::GiveWorldItem(NewPlayer, Item.Item, Item.Count, 0);
	}
	Inventory::GiveWorldItem(NewPlayer, NewPlayer->CosmeticLoadoutPC.Pickaxe->WeaponDefinition, 1, 0);

	std::cout << __int64(GetWorld()->NavigationSystem) << "\n";
	std::cout << (GetWorld()->NavigationSystem ? __int64(((UNavigationSystemV1*)GetWorld()->NavigationSystem)->MainNavData) : 0) << "\n";

	/* *((BYTE*)(GetWorld()->NavigationSystem + 920)) &= 0xFD;

	Sub_16BDED0((UNavigationSystemV1*)GetWorld()->NavigationSystem);

	*/

	auto Pawn = (AFortPlayerPawnAthena*)GameMode->SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetTransform());

	NewPlayer->GetQuestManager(GameMode->AssociatedSubGame)->InitializeQuestAbilities(Pawn);

	return Pawn;
}

void GameMode::HandleStartingNewPlayer(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PC) {
	
	auto PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;

	return HandleStartingNewPlayer_OG(GameMode, PC);
}