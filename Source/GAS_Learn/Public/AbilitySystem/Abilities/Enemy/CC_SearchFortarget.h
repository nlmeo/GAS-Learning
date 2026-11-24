// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "Tasks/AITask_MoveTo.h"
#include "CC_SearchFortarget.generated.h"


namespace EPathFollowingResult
{
	enum Type : int;
}

class UAbilityAsync_WaitGameplayEvent;
class AAIController;
class ACC_EnemyCharacter;
class ACC_BaseCharacter;

UCLASS()
class GAS_LEARN_API UCC_SearchFortarget : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:

	UCC_SearchFortarget();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	TWeakObjectPtr<ACC_EnemyCharacter> OwningEnemy;
	TWeakObjectPtr<AAIController> OwningController;
	TWeakObjectPtr<ACC_BaseCharacter> TargetCharacter;

private:

	bool CacheOwningData();
	bool CanContinueSearch();
	bool CreateAsyncWaitGameplayTagEvenTask();
	void MoveToTargetAndAttack();
	UPROPERTY()
	TObjectPtr<UAbilityAsync_WaitGameplayEvent> WaitGameplayEventTask;

	TWeakObjectPtr<UAITask_MoveTo> MoveTask;
	bool bSearchScheduled = false;

	UFUNCTION()
	void EndAttackEventReceived(FGameplayEventData EventData);

	void StartSearch();

	UFUNCTION()
	void Search();

	UFUNCTION()
	void WaitAttackTarget( TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController);

	UFUNCTION()
	void Attack();
};
