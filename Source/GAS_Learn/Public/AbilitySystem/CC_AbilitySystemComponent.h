// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CC_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAS_LEARN_API UCC_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int Level);
	
	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void AddToAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int AddLevel = 1);

private:
	void HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec);
	
	
};
