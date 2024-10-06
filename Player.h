#pragma once
#include "framework.h"
#include "Abilities.h"

inline void (*ServerReadyToStartMatch_OG)(AFortPlayerController* Controller);
void ServerReadyToStartMatch(AFortPlayerControllerAthena* Controller);

void ServerAcknowledgePossesion(AFortPlayerController* Controller, APawn* Pawn);

void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Component, FRotator ClientRotation);

void ServerHandlePickup(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, float InFlyTime, const struct FVector& InStartDirection, bool bPlayPickupSound);

inline char (*CompletePickupAnimation_OG)(AFortPickup* Pickup);
char CompletePickupAnimation(AFortPickup* Pickup);

AFortWeapon* ServerExecuteInventoryItem(AFortPlayerController* Controller, FGuid ItemGuid);

inline void (*ServerAttemptInventoryDrop_OG)(AFortPlayerController* Controller, const struct FGuid& ItemGuid, int32 Count, bool bTrash);
void ServerAttemptInventoryDrop(AFortPlayerController* Controller, const struct FGuid& ItemGuid, int32 Count, bool bTrash);

inline void (*ServerAttemptInteract_OG)(UFortControllerComponent_Interaction* Component, class AActor* ReceivingActor, class UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, class UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId);
void ServerAttemptInteract(UFortControllerComponent_Interaction* Component, class AActor* ReceivingActor, class UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, class UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId);

inline void (*ClientOnPawnDied_OG)(AFortPlayerControllerAthena* Controller, const struct FFortPlayerDeathReport& DeathReport);
void ClientOnPawnDied(AFortPlayerControllerAthena* Controller, const struct FFortPlayerDeathReport& DeathReport);

inline void (*OnDamageServer_OG)(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);
void OnDamageServer(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);

inline void (*ServerCreateBuildingActor_OG)(AFortPlayerControllerAthena* Controller, const struct FCreateBuildingActorData& CreateBuildingData);
void ServerCreateBuildingActor(AFortPlayerControllerAthena* Controller, const struct FCreateBuildingActorData& CreateBuildingData);

inline void (*ServerBeginEditingBuildingActor_OG)(AFortPlayerControllerAthena* Controller, ABuildingSMActor* BuildingActorToEdit);
void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* Controller, ABuildingSMActor* BuildingActorToEdit);

void ServerEditBuildingActor(AFortPlayerControllerAthena* Controller, class ABuildingSMActor* BuildingActorToEdit, TSubclassOf<class ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored);

void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* ActorToStopEditing);