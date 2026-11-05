// Copyright nlmeo


#include "GAS_Learn/Public/Characters/CC_PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/CC_PlayerState.h"


ACC_PlayerCharacter::ACC_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


}

UAbilitySystemComponent* ACC_PlayerCharacter::GetAbilitySystemComponent() const
{
	ACC_PlayerState* CCPlayerState = Cast<ACC_PlayerState>(GetPlayerState());
	if (!IsValid(CCPlayerState)) return nullptr;

	return CCPlayerState->GetAbilitySystemComponent();

}

UAttributeSet* ACC_PlayerCharacter::GetAttributeSet() const
{
	ACC_PlayerState* CCPlayerState = Cast<ACC_PlayerState>(GetPlayerState());
	if (!IsValid(CCPlayerState)) return nullptr;

	return CCPlayerState->GetAttributeSet();
}



// 服务器在 Controller 接管角色时调用；此时可通过 PlayerState 获取 ASC。
void ACC_PlayerCharacter::PossessedBy(AController* EventController)
{
	Super::PossessedBy(EventController);
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;

	// ASC 由 PlayerState 持有（OwnerActor），当前角色负责表现和执行能力（AvatarActor）。
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(),this);
	// 授予初始技能、应用初始属性效果由服务器负责，相关结果再通过 GAS 复制给客户端。
	GiveStartupAbilities();
	InitializeAttributes();
	// 通知服务器本地的监听者：ASC 已完成这里的初始化，可以绑定属性变化等回调。
	// 监听服务器上的本地玩家 UI 也可能依赖此通知。
	// Broadcast 是本地委托调用，不是 RPC，不会自动通知远程客户端的监听者。
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());

	UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(CC_AttributeSet)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	
}

// 客户端的 PlayerState 引用复制更新时调用（不保证一生只调用一次）。
// 客户端不走服务器的 PossessedBy 初始化流程，因此需要在这里初始化自己的 ASC ActorInfo。
void ACC_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (!IsValid(GetAbilitySystemComponent())) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(),this);
	// 通知这个客户端本地的监听者（例如 UI）：现在可以获取 ASC、AttributeSet 并绑定回调。
	// 两处广播分别服务于服务器和客户端各自的对象实例，并非向同一批监听者固定广播两次。
	// 此通知只表示本地 ASC ActorInfo 已就绪，不保证服务器的初始属性值此刻已全部复制到达。
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(), GetAttributeSet());
	
	UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	if (!IsValid(CC_AttributeSet)) return;
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	
}
