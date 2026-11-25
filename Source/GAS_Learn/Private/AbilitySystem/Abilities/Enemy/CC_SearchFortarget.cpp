// Copyright nlmeo


#include "AbilitySystem/Abilities/Enemy/CC_SearchFortarget.h"

#include "AIController.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Characters/CC_EnemyCharacter.h"
#include "GameplayTags/CC_Tags.h"
#include "Tasks/AITask_MoveTo.h"
#include "Utils/CC_BlueprintLibrary.h"

UCC_SearchFortarget::UCC_SearchFortarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	bActivateBlueprintOnActivation = false;
	FGameplayTagContainer SearchTags = GetAssetTags();
	SearchTags.AddTag(CC_Tags::CC_Abilities::SearchForTarget);
	SetAssetTags(SearchTags);

}

void UCC_SearchFortarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//seq 1
	if (!CacheOwningData())
	{
		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,   // 同步能力结束
			true);  // 本次按取消处理
		return;
	}

	//seq 2
	if (!CreateAsyncWaitGameplayTagEvenTask())
	{
		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,   // 同步能力结束
			true);  // 本次按取消处理
		return;
	}
	//seq 3
	StartSearch();

}

bool UCC_SearchFortarget::CacheOwningData()
{
	OwningController.Reset();
	OwningEnemy = Cast<ACC_EnemyCharacter>(GetAvatarActorFromActorInfo());
	if (!OwningEnemy.IsValid()) return false;

	OwningController = Cast<AAIController>(OwningEnemy->GetController());
	return OwningController.IsValid();
}

bool UCC_SearchFortarget::CreateAsyncWaitGameplayTagEvenTask()
{
	if (!OwningEnemy.IsValid()) return false;

	// Async Action 不随 Ability 自动结束，保存引用并在 EndAbility 中取消。
	if (IsValid(WaitGameplayEventTask))
	{
		WaitGameplayEventTask->Cancel();
	}
	WaitGameplayEventTask = UAbilityAsync_WaitGameplayEvent::WaitGameplayEventToActor(
		OwningEnemy.Get(), CC_Tags::Events::Enemy::EndAttack);
	if (!IsValid(WaitGameplayEventTask)) return false;

	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::EndAttackEventReceived);
	static_cast<UAbilityAsync*>(WaitGameplayEventTask.Get())->Activate();
	return true;

}

void UCC_SearchFortarget::EndAttackEventReceived(FGameplayEventData EventData)
{
	if (OwningEnemy.IsValid() && !OwningEnemy->bIsBeingLaunched)
	{
		StartSearch();
	}

}



void UCC_SearchFortarget::StartSearch()
{
	if (!CanContinueSearch()) return;
	if (bSearchScheduled) return;
	if (bDrawDebugs && GEngine) GEngine->AddOnScreenDebugMessage(-1,3.f, FColor::Red,FString::Printf(TEXT("GA_CC_SearchForTarget:Starting Search")));

	//设置的时候需要注意
	const float WaitSeconds = FMath::RandRange(OwningEnemy->MinAttackDelay,OwningEnemy->MaxAttackDelay);

	// ReadyForActivation 会将任务登记到 Ability 的活动任务列表，无需成员保活。
	UAbilityTask_WaitDelay* SearchDelayTask = UAbilityTask_WaitDelay::WaitDelay(this,WaitSeconds);
	SearchDelayTask->OnFinish.AddDynamic(this,&ThisClass::Search);
	bSearchScheduled = true;
	SearchDelayTask->ReadyForActivation();
}



void UCC_SearchFortarget::Search()
{
	bSearchScheduled = false;
	if (!CanContinueSearch()) return;

	FClosestActorWithTagResult Result= UCC_BlueprintLibrary::FindClosestActorWithTag(OwningEnemy.Get(),OwningEnemy->GetActorLocation(),CrashTags::PlayerTag);

	TargetCharacter = Cast<ACC_BaseCharacter>(Result.Actor);
	OwningEnemy->AimTarget = TargetCharacter.Get();
	if (!TargetCharacter.IsValid())
	{
		StartSearch();
		return;
	}

	if (TargetCharacter->IsAlive())
	{
		MoveToTargetAndAttack();
	}
	else
	{
		StartSearch();
	}

}

void UCC_SearchFortarget::MoveToTargetAndAttack()
{
	if (!CanContinueSearch()) return;
	// 目标失效时重新搜索，避免把空目标解释为移动到 FVector::ZeroVector。
	if (!TargetCharacter.IsValid() || !TargetCharacter->IsAlive())
	{
		StartSearch();
		return;
	}

	//如果执行期间遇到ai正在移动，取消上一个任务创建新的移动任务
	if (MoveTask.IsValid())
	{
		MoveTask->OnMoveTaskFinished.RemoveAll(this);
		if (!MoveTask->IsFinished())
		{
			MoveTask->EndTask();
		}
		MoveTask.Reset();
	}

	MoveTask = UAITask_MoveTo::AIMoveTo
	(
		OwningController.Get(),
		FVector(),
		TargetCharacter.Get(),
		OwningEnemy->AcceptRadius
	);

	if (!MoveTask.IsValid())
	{
		StartSearch();
		return;
	}

	MoveTask->OnMoveTaskFinished.AddUObject(this,&ThisClass::WaitAttackTarget);
	MoveTask->ReadyForActivation();
}


bool UCC_SearchFortarget::CanContinueSearch()
{
	if (!IsActive()) return false;

	// 延迟及移动回调发生时，敌人可能已经死亡，或 Controller 已失效。
	if (!OwningEnemy.IsValid() || !OwningController.IsValid() || !OwningEnemy->IsAlive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return false;
	}

	return true;
}

void UCC_SearchFortarget::WaitAttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	if (!CanContinueSearch())
	{
		return;
	}

	if (Result != EPathFollowingResult::Success || AIController != OwningController.Get()
		|| !TargetCharacter.IsValid() || !TargetCharacter->IsAlive())
	{
		StartSearch();
		return;
	}


	OwningEnemy->RotateToTarget(TargetCharacter.Get());
	UAbilityTask_WaitDelay* AttackTask = UAbilityTask_WaitDelay::WaitDelay(this,OwningEnemy->GetTimelineLength());
	AttackTask->OnFinish.AddDynamic(this,&ThisClass::Attack);
	AttackTask->ReadyForActivation();
}

void UCC_SearchFortarget::Attack()
{
	if (!CanContinueSearch()) return;
	// 转向延迟期间目标可能死亡或被销毁，不能继续等待一个不会发生的攻击结束事件。
	if (!TargetCharacter.IsValid() || !TargetCharacter->IsAlive())
	{
		StartSearch();
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const FGameplayTag AttackTag = CC_Tags::CC_Abilities::Enemy::Attack;
	if (!IsValid(ASC) || !ASC->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer()))
	{
		StartSearch();
	}

}


void UCC_SearchFortarget::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	bSearchScheduled = false;
	// MoveTo 属于 Controller，不会随当前 Ability 自动结束。
	if (MoveTask.IsValid())
	{
		MoveTask->OnMoveTaskFinished.RemoveAll(this);
		if (!MoveTask->IsFinished())
		{
			MoveTask->EndTask();
		}
	}
	MoveTask.Reset();
	TargetCharacter.Reset();
	if (OwningEnemy.IsValid()) OwningEnemy->AimTarget = nullptr;

	if (IsValid(WaitGameplayEventTask))
	{
		WaitGameplayEventTask->Cancel();
		WaitGameplayEventTask = nullptr;
	}

	// Super 会结束当前 Ability 所属的活动任务，包括尚未完成的 WaitDelay。
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
