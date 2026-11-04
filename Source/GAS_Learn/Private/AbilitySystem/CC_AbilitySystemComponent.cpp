// Copyright nlmeo


#include "AbilitySystem/CC_AbilitySystemComponent.h"

#include "GameplayTags/CC_Tags.h"

void UCC_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	if (!IsValid(AbilitySpec.Ability)) return;

	HandleAutoActivatedAbility(AbilitySpec);

}

void UCC_AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	/** Used to stop us from removing abilities from an ability system component while we're iterating through the abilities */
	FScopedAbilityListLock ActiveScopedAbilityListLock(*this);

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		HandleAutoActivatedAbility(AbilitySpec);
	}
}

void UCC_AbilitySystemComponent::HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
	{
		if (Tag.MatchesTagExact(CC_Tags::CC_Abilities::ActiveOnGiven))
		{
			//if (!IsValid(AbilitySpec.Ability) || AbilitySpec.IsActive())

			TryActivateAbility(AbilitySpec.Handle);
			return;
		}
	}
}
