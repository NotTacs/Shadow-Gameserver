#pragma once
#include "framework.h"
#include "Abilities.h"

inline void (*ServerReadyToStartMatch_OG)(AFortPlayerController* Controller);
void ServerReadyToStartMatch(AFortPlayerControllerAthena* Controller);

void ServerAcknowledgePossesion(AFortPlayerController* Controller, APawn* Pawn);

void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Component, FRotator ClientRotation);

void ServerExecuteInventoryItem(AFortPlayerController* Controller, FGuid ItemGuid);

inline void (*ServerAttemptInventoryDrop_OG)(AFortPlayerController* Controller, const struct FGuid& ItemGuid, int32 Count, bool bTrash);
void ServerAttemptInventoryDrop(AFortPlayerController* Controller, const struct FGuid& ItemGuid, int32 Count, bool bTrash);

inline void (*ClientOnPawnDied_OG)(AFortPlayerControllerAthena* Controller, const struct FFortPlayerDeathReport& DeathReport);
void ClientOnPawnDied(AFortPlayerControllerAthena* Controller, const struct FFortPlayerDeathReport& DeathReport);

inline void (*OnDamageServer_OG)(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);
void OnDamageServer(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);