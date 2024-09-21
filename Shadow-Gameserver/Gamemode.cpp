#include "Gamemode.h"

bool ReadyToStartMatch_Hook(AAthena_GameMode_C* GM)
{
	AAthena_GameState_C* GameState = reinterpret_cast<AAthena_GameState_C*>(GM->GameState);

	if (!GM->bWorldIsReady)
	{
		UFortPlaylistAthena* Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");

		if (Playlist)
		{
			GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
			GameState->CurrentPlaylistInfo.MarkArrayDirty();

			GameState->OnRep_CurrentPlaylistInfo();

			GameState->CurrentPlaylistId = Playlist->PlaylistId;
			GameState->OnRep_CurrentPlaylistId();

			GM->bWorldIsReady = true;
		}
	}

	if (!GM->bWorldIsReady)
		return false;

	static bool bReadyToListen = false;

	if (!bReadyToListen)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerStartWarmup::StaticClass(), &Actors);
		Actors.Free();

		if (Actors.Num() == 0) // wdym if == 0 bro u just cleared the array?
			return false;

		bReadyToListen = true;
	}

	if (!GameState->MapInfo)
		return false;

	if (!UWorld::GetWorld()->NetDriver)
	{
		FName GameNetDriverName = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
		UWorld::GetWorld()->NetDriver = UEngine::GetEngine()->CreateNetDriver(UWorld::GetWorld(), GameNetDriverName);

		UWorld::GetWorld()->NetDriver->World = UWorld::GetWorld();
		UWorld::GetWorld()->NetDriver->NetDriverName = GameNetDriverName;

		FURL ListenURL;
		ListenURL.Port = 7777;

		FString Error; // never gets used what

		UWorld::GetWorld()->NetDriver->InitListen(UWorld::GetWorld(), ListenURL, false, Error);
		UWorld::GetWorld()->NetDriver->SetWorld(UWorld::GetWorld());

		UWorld::GetWorld()->LevelCollections[0].NetDriver = UWorld::GetWorld()->NetDriver;
		UWorld::GetWorld()->LevelCollections[1].NetDriver = UWorld::GetWorld()->NetDriver;

		SetConsoleTitleA("Listening on Port 7777");
	}
	
	return GM->AlivePlayers.Num() > 0;
}

APawn* SpawnDefaultPawnFor_Hook(AFortGameModeAthena* GM, AFortPlayerControllerAthena* Controller, AActor* StartSpot)
{
	return GM->SpawnDefaultPawnAtTransform(Controller, StartSpot->GetTransform());
}