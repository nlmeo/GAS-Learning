// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CC_GameplayAbility.generated.h"

UCLASS()
class GAS_LEARN_API UCC_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crash|Debug")
	bool bDrawDebugs = false;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// 原生实现可关闭蓝图激活入口，避免迁移后同时运行旧事件图。
	bool bActivateBlueprintOnActivation = true;

};
