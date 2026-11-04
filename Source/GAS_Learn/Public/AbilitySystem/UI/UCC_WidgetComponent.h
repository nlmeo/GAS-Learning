// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "UCC_WidgetComponent.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UCC_AttributeSet;
class UCC_AbilitySystemComponent;
class ACC_BaseCharacter;

UCLASS()
class GAS_LEARN_API UUCC_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay();
private:

	TWeakObjectPtr<ACC_BaseCharacter> CrashCharacter;
	TWeakObjectPtr<UCC_AbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UCC_AttributeSet> AttributeSet;

	void InitAbilitySystemData();
	bool IsASCInitialized() const;
	void InitializeAttributeDelegate();

	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);

	UFUNCTION()
	void BindToAttributeChanges();
};
