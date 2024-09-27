#include "Abilities.h"

void Abilities::GiveAbilitySet(AFortPawn* Pawn, UFortAbilitySet* Set) {
	if (!Pawn || !Set) return;
	for (int i = 0; i < Set->GameplayAbilities.Num(); i++) {
		TSubclassOf<UFortGameplayAbility> GameplayAbility = Set->GameplayAbilities[i];

		FGameplayAbilitySpec Spec{-1,-1,-1};
		Spec.Ability = reinterpret_cast<UFortGameplayAbility*>(GameplayAbility.Get()->DefaultObject);
		Spec.Level = 1;
		Spec.InputID = -1;
		Spec.SourceObject = nullptr;
		Spec.Handle.Handle = rand();
		GiveAbility(Pawn->AbilitySystemComponent, &Spec.Handle, Spec);
	}
}

void Abilities::InternalServerTryActivateAbilityHook(UAbilitySystemComponent* Component, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData)
{
	FGameplayAbilitySpec* Spec = nullptr;
	for (int i = 0; i < Component->ActivatableAbilities.Items.Num(); i++) {
		FGameplayAbilitySpec Item = Component->ActivatableAbilities.Items[i];

		if (Item.Handle.Handle == Handle.Handle) Spec = &Item;
	}

	if (!Spec) {
		Component->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	Spec->InputPressed = true;

	UGameplayAbility* junk = nullptr;

	if (!InternalTryActivateAbility(Component, Handle, PredictionKey, &junk, nullptr, TriggerEventData)) {
		Component->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		Spec->InputPressed = false;
	}
	Component->ActivatableAbilities.MarkItemDirty(*Spec);
}