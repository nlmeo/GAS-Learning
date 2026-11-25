
#include "Characters/CC_EnemyCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "GameplayTags/CC_Tags.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"


ACC_EnemyCharacter::ACC_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UCC_AttributeSet>("AttributeSet");

	Tags.Add(CrashTags::EnemyTag);
}

UAbilitySystemComponent* ACC_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACC_EnemyCharacter::StopMovementUntilLanded()
{
	bIsBeingLaunched = true;

	AAIController* AIController = GetController<AAIController>();
	if (!IsValid(AIController)) return;
	AIController->StopMovement();
	if (!LandedDelegate.IsAlreadyBound(this,&ThisClass::EnableMovementOnLanded))
	{
		LandedDelegate.AddDynamic(this,&ThisClass::EnableMovementOnLanded);
	}

}

void ACC_EnemyCharacter::EnableMovementOnLanded(const FHitResult& HitResult)
{
	bIsBeingLaunched = false;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,CC_Tags::Events::Enemy::EndAttack,FGameplayEventData());
	LandedDelegate.RemoveAll(this);
}

void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(this,this);
	GiveStartupAbilities();
	InitializeAttributes();


	UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(AttributeSet);
	if (!IsValid(CC_AttributeSet)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

}

void ACC_EnemyCharacter::HandleDeath()
{
	Super::HandleDeath();
	// 先结束搜索，避免 StopMovement 的中止回调再启动一轮搜索。
	if (HasAuthority()) UpdateSearchAbility(false);

	AAIController* AIController = GetController<AAIController>();
	if (!IsValid(AIController)) return;

	AIController->StopMovement();


}

void ACC_EnemyCharacter::HandleRespawn()
{
	Super::HandleRespawn();
	if (!HasAuthority()) return;

	// 死亡蓝图在本调用后还会 EndAbility；下一帧再启动，避免仍被死亡状态阻挡。
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::RestartSearchAbility);
}

void ACC_EnemyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bIsBeingLaunched);
	DOREPLIFETIME(ThisClass, AimTarget);
}

void ACC_EnemyCharacter::RestartSearchAbility()
{
	if (HasAuthority() && IsAlive()) UpdateSearchAbility(true);
}

void ACC_EnemyCharacter::UpdateSearchAbility(bool bEnable)
{
	if (!IsValid(AbilitySystemComponent)) return;

	// 按能力标签激活或取消，不依赖具体的技能 C++ 类。
	const FGameplayTagContainer SearchTags = CC_Tags::CC_Abilities::SearchForTarget.GetTag().GetSingleTagContainer();
	if (bEnable)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(SearchTags, false);
	}
	else
	{
		AbilitySystemComponent->CancelAbilities(&SearchTags);
	}
}
