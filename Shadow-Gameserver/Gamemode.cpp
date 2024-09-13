#include "Gamemode.h"

bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
	auto GameState = reinterpret_cast<AFortGameStateAthena*>(GameMode->GameState);
	static bool bPlaylist = false;
	if (!bPlaylist)
	{
		UFortPlaylistAthena* Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
		GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
		GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
		GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
		GameState->CurrentPlaylistInfo.MarkArrayDirty();
		GameState->OnRep_CurrentPlaylistInfo();

		GameState->CurrentPlaylistId = Playlist->PlaylistId;
		GameState->OnRep_CurrentPlaylistId();
		
		bPlaylist = true;
	}

	static bool bFinishedLoading = false;
	if (!bFinishedLoading)
	{
		TArray<AActor*> Warmups;

		UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerStartWarmup::StaticClass(), &Warmups);

		Warmups.Free();

		if (Warmups.Num() == 0) return false;
		bFinishedLoading = true;
	}

	if (!GameState->MapInfo) return false;

	static bool bListening = false;
	if (!bListening)
	{
		AFortOnlineBeaconHost* Beacon = Misc::SpawnActor<AFortOnlineBeaconHost>({});
		
		Beacon->ListenPort = 7776;

		Imports::InitHost(Beacon);
		Imports::PauseBeaconRequests(Beacon, false);


		FName NetDriverDefinition = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
		Beacon->NetDriver->NetDriverName = NetDriverDefinition;
		UWorld::GetWorld()->NetDriver = Beacon->NetDriver;

		FURL URL = {};
		URL.Port = 7777;
		FString Error;

		Imports::InitListen(UWorld::GetWorld()->NetDriver, UWorld::GetWorld(), URL, false, Error);

		Imports::SetWorld(UWorld::GetWorld()->NetDriver, UWorld::GetWorld());

		SetConsoleTitleA("Listening");

		auto TS = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
		auto DR = 60.f;

		GameState->WarmupCountdownEndTime = TS + DR;
		GameMode->WarmupCountdownDuration = DR;
		GameState->WarmupCountdownStartTime = TS;
		GameMode->WarmupEarlyCountdownDuration = DR;

		for (FLevelCollection Collection : UWorld::GetWorld()->LevelCollections)
		{
			Collection.NetDriver = UWorld::GetWorld()->NetDriver;
		}

		GameMode->bWorldIsReady = true;
		bListening = true;
	}

	bool Ret = GameMode->AlivePlayers.Num() > 0;
	if (!Ret)
	{
		auto TS = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
		auto DR = 60.f;

		GameState->WarmupCountdownEndTime = TS + DR;
		GameMode->WarmupCountdownDuration = DR;
		GameState->WarmupCountdownStartTime = TS;
		GameMode->WarmupEarlyCountdownDuration = DR;
	}

	return Ret;
}

APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* Controller, AActor* StartSpot)
{
	std::cout << "SpawnDefaultPawnFor";
	return GameMode->SpawnDefaultPawnAtTransform(Controller, StartSpot->GetTransform());
}