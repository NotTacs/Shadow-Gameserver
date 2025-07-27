
#include "Bots.h"

#include <intrin.h>

void GiveItem(AFortAthenaAIBotController* Controller, UFortItemDefinition* Def, int count, int Level) {
	UFortWorldItem* Item = (UFortWorldItem*)Def->CreateTemporaryItemInstanceBP(count, Level);
	if (!Item) return;
	if (Controller->Inventory) {
		Item->OwnerInventory = Controller->Inventory;
		Controller->Inventory->Inventory.ItemInstances.Add(Item);
		Controller->Inventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		Controller->Inventory->Inventory.MarkItemDirty(Item->ItemEntry);
		Controller->Inventory->HandleInventoryLocalUpdate();
	}
}

FGuid GetGuid(AFortAthenaAIBotController* Controller, UFortItemDefinition* Def) {
	for (int i = 0; i < Controller->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
		FFortItemEntry Entry = Controller->Inventory->Inventory.ReplicatedEntries[i];

		if (Entry.ItemDefinition == Def) {
			return Entry.ItemGuid;
		}
		else {
			continue;
		}
	}
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

	//MangElevators
	static UFortAthenaAIBotEvaluator* DBNO = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/MANG_Evaluator_DBNO.MANG_Evaluator_DBNO_C");
	static UFortAthenaAIBotEvaluator* Revive = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/MANG_Evaluator_Revive.MANG_Evaluator_Revive_C");
	static UFortAthenaAIBotEvaluator* StressTag = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/Mang_Evaluator_StressTag.Mang_Evaluator_StressTag_C");
	static UFortAthenaAIBotEvaluator* PropagateAwareness = (UFortAthenaAIBotEvaluator*)StaticLoadObject<UBlueprintGeneratedClass>("/Game/Athena/AI/MANG/Evaluators/MANG_Evaluator_PropagateAwareness.MANG_Evaluator_PropagateAwareness_C");

	if (InBotData->CharacterCustomization->CustomizationLoadout.Character->GetName() == "CID_556_Athena_Commando_F_RebirthDefaultA") {
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

	AFortPlayerPawnAthena* Pawn = SpawnBot_OG(BotManager, InSpawnLocation, InSpawnRotation, InBotData, InRuntimeBotData);

	Pawn->bCanBeDamaged = true;
	Pawn->SetMaxHealth(100.f);
	Pawn->SetHealth(Pawn->GetMaxHealth());

	ABP_PhoebeController_NonParticipant_C* Controller = (ABP_PhoebeController_NonParticipant_C*)Pawn->Controller;
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

	//Very Very HardCoded
	Controller->CosmeticLoadoutBC = InBotData->CharacterCustomization->CustomizationLoadout;
	Pawn->CosmeticLoadout = InBotData->CharacterCustomization->CustomizationLoadout;
	Pawn->OnRep_CosmeticLoadout();

	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)Controller->PlayerState;
	PlayerState->HeroType = Controller->CosmeticLoadoutBC.Character->HeroDefinition;

	for (int i = 0; i < PlayerState->HeroType->Specializations.Num(); i++) {
		TSoftObjectPtr<UFortHeroSpecialization> Specialization = PlayerState->HeroType->Specializations[i];

		UFortHeroSpecialization* Spec = Specialization.Get();
		if (!Spec) {
			Spec = StaticLoadObject<UFortHeroSpecialization>(
				UKismetStringLibrary::Conv_NameToString(Specialization.ObjectID.AssetPathName).ToString());
		}

		for (const TSoftObjectPtr<UCustomCharacterPart>& CharacterPart : Spec->CharacterParts) {
			UCustomCharacterPart* CP = CharacterPart.Get();
			if (!CP) {
				std::string Part = UKismetStringLibrary::Conv_NameToString(CharacterPart.ObjectID.AssetPathName).ToString();
				CP = StaticLoadObject<UCustomCharacterPart>(Part);
			}
			Pawn->ServerChoosePart(CP->CharacterPartType, CP);
		}
	}

	Controller->Inventory = SpawnActor<AFortInventory>({}, Controller);
	Controller->Inventory->InventoryType = EFortInventoryType::World;
	Controller->Inventory->Inventory = FFortItemList();
	Controller->Inventory->Inventory.ReplicatedEntries = TArray<struct FFortItemEntry>();
	Controller->Inventory->Inventory.ItemInstances = TArray<class UFortWorldItem*>();
	Controller->Inventory->SetOwner(Controller);

	//Controller->BehaviorTree = InBotData->BehaviorTree;
	Controller->Skill = InBotData->SkillLevel;
	if (Controller->Blackboard)
	{
		std::cout << "Blackboard: " << Controller->Blackboard->GetName() << std::endl;
	}
	Controller->UseBlackboard(InBotData->BehaviorTree->BlackboardAsset, &Controller->Blackboard);
	std::cout << "Blackboard1: " << Controller->Blackboard1->GetName() << std::endl;
	std::cout << "Blackboard: " << Controller->Blackboard->GetName() << std::endl;
	Controller->RunBehaviorTree(Controller->BehaviorTree);

	std::cout << "VTable: " << __int64(Controller->VTable) - ImageBase << std::endl;

	if (Controller->CachedPatrollingComponent) {
		Controller->CachedPatrollingComponent->CachedBotController = Controller;
		TArray<AActor*> Paths;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFortAthenaPatrolPath::StaticClass(), &Paths);

		for (AActor* Path : Paths) {
			AFortAthenaPatrolPath* PatrolPath = (AFortAthenaPatrolPath*)Path;
			if (PatrolPath->PatrolPoints[0]->K2_GetActorLocation() == InSpawnLocation) {
				Controller->CachedPatrollingComponent->PatrolPath = PatrolPath;
				break;
			}
		}

		Paths.Free();
	}

	if (Controller->CachedPatrollingComponent && Controller->CachedPatrollingComponent->PatrolPath) {
		std::cout << "PatrolPath: " << Controller->CachedPatrollingComponent->PatrolPath->GetName() << std::endl;
	}
	else {
		std::cout << "CachedComponent: " << Controller->CachedPatrollingComponent->GetName() << std::endl;
	}

	Controllers.push_back(Controller);

	static bool (*IsCompatibleWith)(UBlackboardComponent*, UBlackboardData*) = decltype(IsCompatibleWith)(ImageBase + 0x4902D50);

	bool IsCompatible = IsCompatibleWith(Controller->Blackboard, InBotData->BehaviorTree->BlackboardAsset);

	std::cout << "IsCompatible: " << IsCompatible << std::endl;

	static UObject* (*StaticConstructObject_Internal)(UClass*, UObject*, FName, EObjectFlags, EInternalObjectFlags, UObject*, bool, void*, bool, UPackage*) = decltype(StaticConstructObject_Internal)(ImageBase + 0x2DBFAB0);

	if (Controller->IsDefaultObject())
	{
				std::cout << "Controller is a default object, cannot create components." << std::endl;
	}

	if (Controller->BrainComponent)
	{
		std::cout << "BrainComponent: " << Controller->BrainComponent->GetName() << std::endl;
	}

	if (Controller->PerceptionComponent)
	{
		std::cout << "PerceptionComponent: " << Controller->PerceptionComponent->GetName() << std::endl;
	}

	if (auto Manager = Controller->CachedBotManager)
	{
		std::cout << "CachedBotManager: " << Manager->GetName() << std::endl;
		bool Idk = *reinterpret_cast<bool*>(__int64(Manager) + 0x4D0);
		std::cout << "Idk: " << Idk << std::endl;
	
	}

	auto& NavAgentprops = Controller->Character->CharacterMovement->NavAgentProps;
	std::cout << "AgentHeight : " << NavAgentprops.AgentHeight;
	std::cout << "AgentRadius: " << NavAgentprops.AgentRadius;
	

	printf("Pawn: %s\n", Pawn->GetName().c_str());

	static INavigationDataInterface* (*GetNavDataForActor)(UObject*) = decltype(GetNavDataForActor)(ImageBase + 0x4814A90);
	INavigationDataInterface* NavData = GetNavDataForActor(Pawn);
	
	if (NavData)
	{
		printf("NavData\n");

	}

	if (InBotData->StartupInventory)
	{
		for (int i = 0; i < InBotData->StartupInventory->Items.Num(); i++)
		{
			UFortItemDefinition* Item = InBotData->StartupInventory->Items[i];

			if (!Item)
				continue;
			GiveItem(Controller, Item, 1, 0);
			FName MiscItem = UKismetStringLibrary::Conv_StringToName(L"Weapon.Meta.MiscWrapped");
			if (Item->IsA(UFortGadgetItemDefinition::StaticClass()) && !Item->GetName().contains("KeyCard")) {
				Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Item, GetGuid(Controller, Item));
			}
			if (Item->IsA(UFortWeaponRangedItemDefinition::StaticClass()) && Item->GameplayTags.GameplayTags[0].TagName != MiscItem)
			{
				Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Item, GetGuid(Controller, Item));
			}
		}
	}

	//std::cout << Controller->BotPlayerName.ToString() << std::endl;
	//GameMode->ChangeName(Controller, UKismetTextLibrary::Conv_TextToString(InBotData->BotNameSettings->OverrideName), true);
	//Controller->BotPlayerName = UKismetTextLibrary::Conv_TextToString(InBotData->BotNameSettings->OverrideName);

	return Pawn;
}