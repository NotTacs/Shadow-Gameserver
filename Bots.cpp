#include "Bots.h"
#include <intrin.h>
void DumpAllPatrolPathObjects()
{
	static std::ofstream Log("PatrolPath.log");
	for (int i = 0; i < UObject::GObjects->Num(); i++) {
		UObject* Object = UObject::GObjects->GetByIndex(i);

		if (!Object) continue;

		if (Object->IsA(AFortAthenaPatrolPath::StaticClass())) {
			Log << "[PatrolPath]: " << Object->GetFullName() << std::endl;
		}
	}
	Log.close();
}

class AllBots {
public:
	TArray<AFortAthenaAIBotController*> AiControllers;
	TArray<AFortPlayerPawnAthena*> AiPawns;

	AllBots(AFortPlayerPawnAthena* P, AFortAthenaAIBotController* C) {
		AiControllers.Add(C);
		AiPawns.Add(P);
	}

	inline void AssignPatrolToNPC() {

	}
};

AFortPlayerPawnAthena* (*SpawnBot_OG)(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData);
AFortPlayerPawnAthena* SpawnBot(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData) {
	auto GameState = reinterpret_cast<AFortGameStateAthena*>(UWorld::GetWorld()->GameState);
	auto GameMode = reinterpret_cast<AFortGameModeAthena*>(GameState->AuthorityGameMode);
	if (InBotData->GetFullName().contains("Engine")) {
		__int64 Address = __int64(_ReturnAddress()) - ImageBase;
		std::cout << "Address: " << Address << std::endl;
		return SpawnBot_OG(BotManager, InSpawnLocation, InSpawnRotation, InBotData, InRuntimeBotData);
	}

	AFortPlayerPawnAthena* Pawn = 
		GameMode->ServerBotManager->CachedBotMutator->SpawnBot(InBotData->PawnClass, nullptr, InSpawnLocation, InSpawnRotation, true);
	AFortAthenaAIBotController* Controller = (AFortAthenaAIBotController*)Pawn->Controller;
	AllBots(Pawn, Controller);
	std::cout << "AiEvaluators For {}" << Pawn->GetName() << " " << Controller->AIEvaluators.Num() << std::endl;
	std::cout << Controller->BotPlayerName.ToString() << std::endl;

	for (int i = 0; i < ((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->RegisteredAISpawners.Num(); i++) {
		AActor* RegisteredAiSpawner = ((UAthenaAISystem*)UWorld::GetWorld()->AISystem)->RegisteredAISpawners[i];
		std::cout << "RegisteredAiSpawner: " << RegisteredAiSpawner << std::endl;
	}
	
	UFunction* Function = UObject::FindObject<UFunction>("UFunction BP_MANG_Spawner.AssignPatrolToNPC");
	if (Function) {
		std::cout << "Found Function" << std::endl;
	}
	return Pawn;
}