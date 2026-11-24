// Copyright nlmeo


#include "Notifies/CC_AN_MeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "KismetTraceUtils.h"
#include "Characters/CC_PlayerCharacter.h"
#include "GameplayTags/CC_Tags.h"
#include "Kismet/KismetMathLibrary.h"



void UCC_AN_MeleeAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	//Check Validity for Mesh and Owner
	if (!IsValid(MeshComp)) return;
	if (!IsValid(MeshComp->GetOwner())) return;
	//DoSphereTrace
	TArray<FHitResult> OutHits = DoSphereTrace(MeshComp);
	//Send Event To Actor
	SendEventToActor(OutHits,MeshComp);
}

TArray<FHitResult> UCC_AN_MeleeAttack::DoSphereTrace(USkeletalMeshComponent* MeshComp) const
{
	TArray<FHitResult> OutHits;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());

	//FVector Start, End;
	const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
	const FVector Start = SocketTransform.GetLocation();

	//const FVector ExtendedSocketDirection = SocketTransform.GetRotation().GetForwardVector() * ExtensionOffset;
	const FVector ExtendedSocketDirection = UKismetMathLibrary::GetForwardVector(SocketTransform.GetRotation().Rotator()) * ExtensionOffset;

	const FVector End = Start - ExtendedSocketDirection;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn,ECR_Block);

	UWorld* World = GEngine->GetWorldFromContextObject(MeshComp, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return OutHits;

	const bool bHit = World->SweepMultiByChannel
	(
		OutHits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereTraceRadius),
		Params,
		ResponseParams
	);

	if (bDrawDebugs)
	{
		DrawDebugSphereTraceMulti
		(
			World,
			Start,
			End,
			SphereTraceRadius,
			EDrawDebugTrace::ForDuration,
			bHit,
			OutHits,
			FLinearColor::Red,
			FLinearColor::Green,
			3.f
			);
	}


	return OutHits;
}

void UCC_AN_MeleeAttack::SendEventToActor(const TArray<FHitResult>& HitResults, USkeletalMeshComponent* MeshComp) const
{
	if (!IsValid(MeshComp)) return;
	if (HitResults.Num() == 0) return;

	for (const FHitResult& HitResult : HitResults)
	{
		ACC_PlayerCharacter* PlayerCharacter = Cast<ACC_PlayerCharacter>(HitResult.GetActor());
		if (!IsValid(PlayerCharacter)) continue;
		if (!PlayerCharacter->IsAlive()) continue;
		UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
		if (!IsValid(ASC)) continue;

		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddHitResult(HitResult);

		FGameplayEventData Payload;
		Payload.ContextHandle = ContextHandle;
		Payload.Target = PlayerCharacter;
		Payload.Instigator = MeshComp->GetOwner();

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(),CC_Tags::Events::Enemy::MeleeTraceHit,Payload);
	}
}
