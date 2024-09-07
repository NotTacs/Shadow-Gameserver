#include "GameSessions.h"

void GetAllDedicatedAthenaGameSessionsInWorld(UWorld* World)
{
	TArray<AActor*> GameSessions;
	UGameplayStatics::GetAllActorsOfClass(World, AFortGameSessionDedicatedAthena::StaticClass(), &GameSessions);
	std::cout << "[LogAllGameSessions]: DedicatedSessions: " << GameSessions.Num() << "\n";
}

void GetAllFortGameSessionsInWorld(UWorld* World)
{
	TArray<AActor*> GameSessions;
	UGameplayStatics::GetAllActorsOfClass(World, AFortGameSession::StaticClass(), &GameSessions);
	std::cout << "[LogAllGameSessions: FortGameSessions: " << GameSessions.Num() << "\n";
}