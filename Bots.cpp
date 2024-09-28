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


AFortPlayerPawnAthena* SpawnBot(UFortServerBotManagerAthena* BotManager, const struct FVector& InSpawnLocation, const struct FRotator& InSpawnRotation, const class UFortAthenaAIBotCustomizationData* InBotData, const struct FFortAthenaAIBotRunTimeCustomizationData& InRuntimeBotData) {
	auto GameState = reinterpret_cast<AFortGameStateAthena*>(UWorld::GetWorld()->GameState);
	auto GameMode = reinterpret_cast<AFortGameModeAthena*>(GameState->AuthorityGameMode);
	if (__int64(_ReturnAddress()) - __int64(GetModuleHandleW(0)) == 0x1A4153F)
		return SpawnBot_OG(BotManager, InSpawnLocation, InSpawnRotation, InBotData, InRuntimeBotData);

	if (!GameMode->ServerBotManager->CachedBotMutator) {
		printf("Null Bot Mutator");
		return SpawnBot_OG(BotManager, InSpawnLocation, InSpawnRotation, InBotData, InRuntimeBotData);
	}

	FTransform Transform;
	Transform.Translation = InSpawnLocation;
	Transform.Rotation = ConvertRotToQuat(InSpawnRotation);
	Transform.Scale3D = FVector(1, 1, 1);
	AActor* SpawnLocator = SpawnActor<ADefaultPawn>(Transform);

	//MangElevators
	static UFortAthenaAIBotEvaluator* DBNO = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/MANG_Evaluator_DBNO.MANG_Evaluator_DBNO_C");
	static UFortAthenaAIBotEvaluator* Revive = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/MANG_Evaluator_Revive.MANG_Evaluator_Revive_C");
	static UFortAthenaAIBotEvaluator* StressTag = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/Mang_Evaluator_StressTag.Mang_Evaluator_StressTag_C");
	static UFortAthenaAIBotEvaluator* PropagateAwareness = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/MANG_Evaluator_PropagateAwareness.MANG_Evaluator_PropagateAwareness_C");

	//std:: cout << InRuntimeBotData.PredefinedCosmeticSetTag.TagName.ToString() << std::endl;

	
	if (InBotData->CharacterCustomization->CustomizationLoadout.Character->GetFullName().contains("CID_556_Athena_Commando_F_RebirthDefaultA")) {
		UDataTable* Table = StaticLoadObject<UDataTable>("/Game/Athena/AI/MANG/Cosmetic/MANG_Cosmetic_Sets_Default.MANG_Cosmetic_Sets_Default");
		if (Table) {
			for (int i = 0; i < Table->RowMap.Num(); i++) {
				TPair<FName, uint8*> Row = Table->RowMap[i];
				auto Second = (FFortBotCosmeticItemSetDataTableRow*)Row.Second;
				if (Second->SetTag.TagName == InRuntimeBotData.PredefinedCosmeticSetTag.TagName) {
					std::string String = UKismetStringLibrary::Conv_NameToString(Second->CharacterAssetId.PrimaryAssetName).ToString();
					UAthenaCharacterItemDefinition* Character = 
						StaticLoadObject<UAthenaCharacterItemDefinition>("/Game/Athena/Items/Cosmetics/Characters/" + String);
					if (!Character) {
						printf("No Character");
					}
					else {
						InBotData->CharacterCustomization->CustomizationLoadout.Character = Character;
					}
				};
			}
		}
	}

	std::cout << "Character: " << InBotData->CharacterCustomization->CustomizationLoadout.Character->GetFullName() << std::endl;
	
	AFortPlayerPawnAthena* Pawn = 
		GameMode->ServerBotManager->CachedBotMutator->SpawnBot(InBotData->PawnClass, SpawnLocator, InSpawnLocation, InSpawnRotation, true);
	ABP_PhoebeController_NonParticipant_C* Controller = (ABP_PhoebeController_NonParticipant_C*)Pawn->Controller;
	std::cout << "Class: " << Controller->Class->GetFullName() << std::endl;
	AllBots(Pawn, Controller);
	//Hardcoding time
	if (DBNO) {
		Controller->AIEvaluators.Add(DBNO);
	}
	if (Revive) {
		Controller->AIEvaluators.Add(Revive);
	}
	if (StressTag) {
		Controller->AIEvaluators.Add(StressTag);
	}
	if (PropagateAwareness) {
		Controller->AIEvaluators.Add(PropagateAwareness);
	}

	std::cout <<  "Shit: " << Controller->AIEvaluators.Num() << std::endl;

	//Very Very HardCoded
	Controller->CosmeticLoadoutBC.Character = InBotData->CharacterCustomization->CustomizationLoadout.Character;
	Pawn->CosmeticLoadout = InBotData->CharacterCustomization->CustomizationLoadout;
	Pawn->OnRep_CosmeticLoadout();
	((AFortPlayerStateAthena*)Controller->PlayerState)->HeroType = Controller->CosmeticLoadoutBC.Character->HeroDefinition;
	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)Controller->PlayerState;

	for (int i = 0; i < PlayerState->HeroType->Specializations.Num(); i++) {
		TSoftObjectPtr<UFortHeroSpecialization> Specialization = PlayerState->HeroType->Specializations[i];
		
		UFortHeroSpecialization* Spec = Specialization.Get();
		if (!Spec) {
			Spec = StaticLoadObject<UFortHeroSpecialization>(
				UKismetStringLibrary::Conv_NameToString(Specialization.ObjectID.AssetPathName).ToString());
		}

		for (TSoftObjectPtr<UCustomCharacterPart> CharacterPart : Spec->CharacterParts) {
			UCustomCharacterPart* CP = CharacterPart.Get();
			if (!CP) {
				std::string Part = UKismetStringLibrary::Conv_NameToString(CharacterPart.ObjectID.AssetPathName).ToString();
				CP = StaticLoadObject<UCustomCharacterPart>(Part);
			}
			Pawn->ServerChoosePart(CP->CharacterPartType, CP);
		}
	}

	//std::cout << Controller->BotPlayerName.ToString() << std::endl;
	//GameMode->ChangeName(Controller, UKismetTextLibrary::Conv_TextToString(InBotData->BotNameSettings->OverrideName), true);
	//Controller->BotPlayerName = UKismetTextLibrary::Conv_TextToString(InBotData->BotNameSettings->OverrideName);

	SpawnLocator->K2_DestroyActor();
	return Pawn;
}