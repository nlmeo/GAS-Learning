#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "GameFramework/Character.h"

#include "CC_BaseCharacter.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized,UAbilitySystemComponent*, ASC, UAttributeSet*, AS);


UCLASS(Abstract)
class GAS_LEARN_API ACC_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACC_BaseCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const {return nullptr;};
	bool IsAlive() const {return bAlive;}
	void SetAlive(bool bAliveStatus) {bAlive = bAliveStatus;}
	
	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;
	
	UFUNCTION(BlueprintCallable, Category = "Crash|Death")
	virtual void HandleRespawn();
protected:
	void GiveStartupAbilities();
	void InitializeAttributes() const;
	
	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);
	virtual void HandleDeath();
private:

	UPROPERTY(EditdefaultsOnly, Category="Crash|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditdefaultsOnly, Category="Crash|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bAlive = true;
};
