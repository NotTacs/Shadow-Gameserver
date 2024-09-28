#pragma once
#include "framework.h"
#include "Abilities.h"

inline void (*ServerReadyToStartMatch_OG)(AFortPlayerController* Controller);
void ServerReadyToStartMatch(AFortPlayerController* Controller);

void ServerAcknowledgePossesion(AFortPlayerController* Controller, APawn* Pawn);

void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Component, FRotator ClientRotation);

void ServerExecuteInventoryItem(AFortPlayerController* Controller, FGuid ItemGuid);

inline void (*OnDamageServer_OG)(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);
void OnDamageServer(ABuildingSMActor* Object, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);