#include "Gamemode.h"

bool ReadyToStartMatch_Hook(AAthena_GameMode_C* GM)
{
	AAthena_GameState_C* GameState = reinterpret_cast<AAthena_GameState_C*>(GM->GameState);

	static bool bSetupPlaylistData = false;
	if (!bSetupPlaylistData)
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

			bSetupPlaylistData = true;
		}
	}

	if (!bSetupPlaylistData) return false;

	static bool bReadyToListen = false;
	if (!bReadyToListen)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerStartWarmup::StaticClass(), &Actors);
		Actors.Free();
		if (Actors.Num() == 0) return false;
		bReadyToListen = true;
	}

	if (!GameState->MapInfo) return false;

	static bool bStartedListening = false;
	if (!bStartedListening)
	{
		FName GameNetDriver = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
		UWorld::GetWorld()->NetDriver = UEngine::GetEngine()->CreateNetDriver(UWorld::GetWorld(), GameNetDriver);

		SetConsoleTitleA("Listening On Port 7777");
		GM->bWorldIsReady = true;
		bStartedListening = true;
	}

	
	return GM->AlivePlayers.Num() > 0;
}