#pragma once
#include "framework.h"

inline FGameplayAbilitySpecHandle* (*GiveAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, const FGameplayAbilitySpec&) = decltype(GiveAbility)(ImageBase + 0x6b19e0);
inline FGameplayAbilitySpecHandle* (*GiveAbilityAndActivateOnce)(UAbilitySystemComponent*, FGameplayAbilitySpec&, const FGameplayEventData*) = decltype(GiveAbilityAndActivateOnce)(ImageBase + 0x6B1B00);
inline bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) = decltype(InternalTryActivateAbility)(ImageBase + 0x6b33f0);
namespace Abilities
{
	void GiveAbilitySet(AFortPawn* Pawn, UFortAbilitySet* Set);
	void InternalServerTryActivateAbilityHook(UAbilitySystemComponent*, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData);

}

