#include "Looting.h"
#include <random>

std::vector<double> GetCumlativeWeights(std::vector<FFortLootTierData*> LootTiers) {
	std::vector<double> cumulativeWeights;
	double totalweight = 0.0;
	for (FFortLootTierData* Tier : LootTiers) {
		totalweight += Tier->Weight;
		cumulativeWeights.push_back(totalweight);
	}

	return cumulativeWeights;
}

FFortLootTierData* GetChosenLootTierData(std::vector<FFortLootTierData*> LootTiers) {
	auto CumWeights = GetCumlativeWeights(LootTiers);
	std::random_device RD;
	std::mt19937 putyourseedinmyass(RD());
	std::uniform_real_distribution<> dis(0.0, CumWeights.back());

	double grape = dis(putyourseedinmyass);

	for (size_t PDiddy = 0; PDiddy < CumWeights.size(); ++PDiddy) {
		if (grape < CumWeights[PDiddy]) {
			return LootTiers[PDiddy];
		}
	}

	return LootTiers.back(); //Gay Sex if happen
}

std::vector<double> GetCumlativeWeightsP(std::vector<FFortLootPackageData*> LootPackages) {
	std::vector<double> cumulativeWeights;
	double totalweight = 0.0;
	for (FFortLootPackageData* Tier : LootPackages) {
		totalweight += Tier->Weight;
		cumulativeWeights.push_back(totalweight);
	}

	return cumulativeWeights;
}

FFortLootPackageData* GetChosenLootTierDataP(std::vector<FFortLootPackageData*> LootTiers) {
	if (LootTiers.size() != 0) {
		auto CumWeights = GetCumlativeWeightsP(LootTiers);
		std::random_device RD;
		std::mt19937 putyourseedinmyass(RD());
		std::uniform_real_distribution<> dis(0.0, CumWeights.back());

		double grape = dis(putyourseedinmyass);

		for (size_t PDiddy = 0; PDiddy < CumWeights.size(); ++PDiddy) {
			if (grape < CumWeights[PDiddy]) {
				return LootTiers[PDiddy];
			}
		}
	}
	

	return LootTiers.back(); //Gay Sex if happen
}

std::vector<FFortItemEntry> GetItems(FName Name) {
	std::vector<FFortItemEntry> LootDrops;

	static UDataTable* LDataTable = StaticLoadObject<UDataTable>("/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client");
	static UDataTable* PDataTable = StaticLoadObject<UDataTable>("/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client");
	std::vector<FFortLootTierData*> AllLootTierData;
	for (TPair<FName, uint8*> Map : LDataTable->RowMap) {
		FFortLootTierData* LootTierData = (FFortLootTierData*)Map.Second;
		if (LootTierData->TierGroup == Name && LootTierData->Weight != 0 && !LootTierData->LootPackage.ToString().contains("Empty")) {
			AllLootTierData.push_back(LootTierData);
		}
	}

	FFortLootTierData* ChosenLootTierData = GetChosenLootTierData(AllLootTierData);

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