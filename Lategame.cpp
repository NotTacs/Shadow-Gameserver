#include "Lategame.h"

void ApplyAllLategameShit() {
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuildingFoundation::StaticClass(), &Actors);

	auto ChosenFoundation = (ABuildingFoundation*)Actors[rand() % Actors.Num()];

	FVector SafeZoneLocation = ChosenFoundation->K2_GetActorLocation();
	SafeZoneLocation.Z + 10000;

	auto GM = (AFortGameModeAthena*)GetWorld()->AuthorityGameMode;
	GM->SafeZonePhase = 2;
}