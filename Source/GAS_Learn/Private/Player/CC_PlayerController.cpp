// Copyright nlmeo


#include "Player/CC_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Characters/CC_BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameplayTags/CC_Tags.h"

void ACC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	for (UInputMappingContext* Context: InputMappingContexts)
	{
		InputSubsystem->AddMappingContext(Context, 0);
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

	EnhancedInputComponent->BindAction(PrimaryAction,ETriggerEvent::Triggered,this, &ThisClass::Primary);
	EnhancedInputComponent->BindAction(SecondaryAction,ETriggerEvent::Started,this, &ThisClass::Secondary);
	EnhancedInputComponent->BindAction(TertiaryAction,ETriggerEvent::Started,this, &ThisClass::Tertiary);
}

void ACC_PlayerController::Jump()
{
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;


	GetCharacter()->Jump();
}

void ACC_PlayerController::StopJumping()
{
	if (!IsValid(GetCharacter())) return;
	if (!IsAlive()) return;
	GetCharacter()->StopJumping();
}

void ACC_PlayerController::Move(const FInputActionValue& Value)
{

	if (!IsValid(GetPawn())) return;
	if (!IsAlive()) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	//寻找哪个方向是角色的正前方
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);

}

void ACC_PlayerController::Look(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn())) return;
	if (!IsAlive()) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ACC_PlayerController::Primary()
{
	ActivateAbility(CC_Tags::CC_Abilities::Primary);
}

void ACC_PlayerController::Secondary()
{
	ActivateAbility(CC_Tags::CC_Abilities::Secondary);
}

void ACC_PlayerController::Tertiary()
{
	ActivateAbility(CC_Tags::CC_Abilities::Tertiary);
}

void ACC_PlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	// 三个技能输入共用此入口，统一拦截死亡或未持有角色时的技能激活。
	if (!IsAlive()) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!IsValid(ASC)) return;

	ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

bool ACC_PlayerController::IsAlive() const
{
	const ACC_BaseCharacter* ControlledCharacter = Cast<ACC_BaseCharacter>(GetPawn());
	return IsValid(ControlledCharacter) && ControlledCharacter->IsAlive();
}
