
#include "Looting.h"
#include <random>

float GetCumlativeWeights(std::vector<FFortLootTierData*>& LootTiers) {

	float TotalWeight = 0;

	for (auto Item : LootTiers) {
		TotalWeight += Item->Weight;
	}

	return TotalWeight;
}

FFortLootTierData* GetChosenLootTierData(std::vector<FFortLootTierData*>& LootTiers) {
	FFortLootTierData* Data = nullptr;
	if (LootTiers.size() != 0) {
		float CumlativeWeights = GetCumlativeWeights(LootTiers);

		if (CumlativeWeights == 0) {
			printf("Weights are gay???");
		}

		float WeightToUse = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(0, CumlativeWeights);

		for (auto Item : LootTiers) {
			if (WeightToUse <= Item->Weight) {
				Data = Item;
				break;
			}

			WeightToUse -= Item->Weight;
		}
	}


	return Data; //Gay Sex if happen
}

float GetCumlativeWeightsP(std::vector<FFortLootPackageData*> LootPackages) {
	float cumulativeWeights = 0;

	for (auto& Item : LootPackages) {
		cumulativeWeights += Item->Weight;
	}

	return cumulativeWeights;
}

FFortLootPackageData* GetChosenLootTierDataP(std::vector<FFortLootPackageData*> LootPackages) {
	FFortLootPackageData* PackageData = nullptr;
	if (LootPackages.size() != 0) {
		float TotalWeights = GetCumlativeWeightsP(LootPackages);

		float WeightToUse = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(0, TotalWeights);

		for (auto& Package : LootPackages) {
			if (WeightToUse <= Package->Weight) {
				PackageData = Package;
				break;
			}

			WeightToUse -= Package->Weight;
		}
	}
	

	return PackageData; //Gay Sex if happen
}


//ToDo: Rewrite this fucking ass shit
std::vector<FFortItemEntry> GetItems(FName Name) {
	std::vector<FFortItemEntry> LootDrops;

	static UDataTable* LDataTable;
	static UDataTable* PDataTable;
	std::string Test = UKismetStringLibrary::Conv_NameToString(CurrentPlaylist()->LootTierData.ObjectID.AssetPathName).ToString();
	std::string Test2 = UKismetStringLibrary::Conv_NameToString(CurrentPlaylist()->LootPackages.ObjectID.AssetPathName).ToString();
	if (Test.empty() || Test.contains("None")) {
		LDataTable = StaticLoadObject<UDataTable>("/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client");
		PDataTable = StaticLoadObject<UDataTable>("/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client");
	}
	else {
		LDataTable = StaticLoadObject<UDataTable>(Test);
		PDataTable = StaticLoadObject<UDataTable>(Test2);
	}

	if (!LDataTable || !PDataTable) return LootDrops;

	if (Name.ToString() == "None" || Test.empty()) return LootDrops;

	std::string Rizz = "";

	if (!Name.ToString().contains("Athena")) {
		std::cout << "BeforeTierGroup: " << Name.ToString() << "\n";
		Rizz = LootTierGroupModifcation(Name.ToString());

		std::cout << "TierGroupAfter: " << Rizz << std::endl;
	}
	else {
		Rizz = Name.ToString();
	}

	std::vector<FFortLootTierData*> AllLootTierData;
	for (TPair<FName, uint8*> Map : LDataTable->RowMap) {
		FFortLootTierData* LootTierData = (FFortLootTierData*)Map.Second;
		if (LootTierData->TierGroup.ToString() == Rizz && !LootTierData->LootPackage.ToString().contains("Empty") && LootTierData->Weight != 0) {
			AllLootTierData.push_back(LootTierData);
		}
	}

	FFortLootTierData* ChosenLootTierData = GetChosenLootTierData(AllLootTierData);

	if (!ChosenLootTierData) return LootDrops;

	int TLPCWA = 0;
	int TLPMA = 0;

	for (int Vhat = 0; Vhat < ChosenLootTierData->LootPackageCategoryWeightArray.Num(); Vhat++) {
		if (ChosenLootTierData->LootPackageCategoryWeightArray[Vhat] > 0) {
			if (ChosenLootTierData->LootPackageCategoryMaxArray[Vhat] < 0)
				TLPCWA += ChosenLootTierData->LootPackageCategoryWeightArray[Vhat];
		}
	}

	for (int Vhat2 = 0; Vhat2 < ChosenLootTierData->LootPackageCategoryMinArray.Num(); Vhat2++) {
		if (ChosenLootTierData->LootPackageCategoryMinArray[Vhat2] > 0) {
			if (ChosenLootTierData->LootPackageCategoryMaxArray[Vhat2] < 0)
				TLPMA += ChosenLootTierData->LootPackageCategoryWeightArray[Vhat2];
		}
	}

	if (TLPCWA > TLPMA) return GetItems(Name);

	std::vector<FFortLootPackageData*> AllLootPackageData;
	for (TPair<FName, uint8*> Map : PDataTable->RowMap) {
		FFortLootPackageData* LootPackageData = (FFortLootPackageData*)Map.Second;

		if (LootPackageData->LootPackageID == ChosenLootTierData->LootPackage && LootPackageData->Weight != 0) {
			AllLootPackageData.push_back(LootPackageData);
		}
	}

	std::vector<FFortLootPackageData*> LootPackageCalls;

	float NumLootPackageDrops = 1;

	for (float Index = 0; Index < NumLootPackageDrops; Index++) {
		auto LootPackage = AllLootPackageData.at(Index);

		if (LootPackage->LootPackageCall.ToString().empty() && LootPackage->Weight != 0) {
			LootPackageCalls.push_back(LootPackage);
		}
		else {
			for (TPair<FName, uint8*> Map : PDataTable->RowMap) {
				FFortLootPackageData* LootPackageData = (FFortLootPackageData*)Map.Second;

				if (LootPackageData->LootPackageID.ToString() == LootPackage->LootPackageCall.ToString() && LootPackageData->Weight != 0) {
					LootPackageCalls.push_back(LootPackageData);
				}
			}
		}

		FFortLootPackageData* LootPackageCall = GetChosenLootTierDataP(LootPackageCalls);
		if (!LootPackageCall) continue;

		UFortItemDefinition* ItemDef = StaticLoadObject<UFortItemDefinition>(UKismetStringLibrary::Conv_NameToString(LootPackageCall->ItemDefinition.ObjectID.AssetPathName).ToString());

		if (!ItemDef) GetItems(Name);

		FFortItemEntry LootDropEntry{};
		LootDropEntry.ItemDefinition = ItemDef;
		LootDropEntry.Count = LootPackageCall->Count;

		LootDrops.push_back(LootDropEntry);
	}

	return LootDrops;
}