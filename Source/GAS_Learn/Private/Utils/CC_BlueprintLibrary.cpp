// Copyright nlmeo


#include "Utils/CC_BlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/CC_BaseCharacter.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/CC_Tags.h"
#include "Kismet/GameplayStatics.h"


EHitDirection UCC_BlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}
	if (Dot < 0.5f)
	{
		// Either left or right
		const FVector Cross = FVector::CrossProduct(TargetForward,ToInstigator);
		if (Cross.Z < 0.f)
		{
			return EHitDirection::Left;
		}
		return EHitDirection::Right;
	}
	return EHitDirection::Forward;
}

FName UCC_BlueprintLibrary::GetHitDirectionName(const EHitDirection& Direction)
{
	switch (Direction)
	{
		case EHitDirection::Back: return FName("Back");
		case EHitDirection::Left: return FName("Left");
		case EHitDirection::Right: return FName("Right");
		case EHitDirection::Forward: return FName("Front");
		default: return FName("None");
	}
}


FClosestActorWithTagResult UCC_BlueprintLibrary::FindClosestActorWithTag(const UObject* WorldContextObject,
	const FVector& Origin, const FName& Tag, float SearchRange)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(WorldContextObject,Tag,Actors);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor)) continue;
		ACC_BaseCharacter* BaseCharacter = Cast<ACC_BaseCharacter>(Actor);
		if (!IsValid(BaseCharacter) || (IsValid(BaseCharacter) && !BaseCharacter->IsAlive())) continue;

		float TmpDistance = FVector::Distance(Origin,BaseCharacter->GetActorLocation());
		if (TmpDistance > SearchRange) continue;

		if (TmpDistance < ClosestDistance)
		{
			ClosestDistance = TmpDistance;
			ClosestActor = Actor;
		}
	}


	return FClosestActorWithTagResult(ClosestActor,ClosestDistance);

}

void UCC_BlueprintLibrary::SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect>& DamageEffect,
	FGameplayEventData& Payload, const FGameplayTag& DataTag, float DamageNumber,const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	ACC_BaseCharacter* Character = Cast<ACC_BaseCharacter>(Target);
	if (!IsValid(Character)) return;
	if (!Character->IsAlive()) return;

	UCC_AttributeSet* Attributes = Cast<UCC_AttributeSet>(Character->GetAttributeSet());
	UAbilitySystemComponent* TargetASC = Character->GetAbilitySystemComponent();
	if (!IsValid(Attributes) || !IsValid(TargetASC) || !IsValid(DamageEffect)) return;

	Payload.Target = Target;
	if (!Payload.ContextHandle.IsValid()) Payload.ContextHandle = TargetASC->MakeEffectContext();
	if (IsValid(Payload.Instigator.Get()))
	{
		Payload.ContextHandle.AddInstigator(const_cast<AActor*>(Payload.Instigator.Get()), const_cast<AActor*>(Payload.Instigator.Get()));
		if (!Payload.ContextHandle.HasOrigin()) Payload.ContextHandle.AddOrigin(Payload.Instigator->GetActorLocation());
	}
	if (IsValid(OptionalParticleSystem)) Payload.OptionalObject = OptionalParticleSystem;


	//应用GE
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect,1.0f,Payload.ContextHandle);
	if (!SpecHandle.IsValid()) return;
	//通过传入的伤害来动态修改GE影响的值
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTag, -DamageNumber);

	// 先结算伤害，避免死亡事件同步触发复活时，这一击扣到复活后的生命值上。
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	FGameplayTag EventTag;
	if (!EventTagOverride.MatchesTagExact(CC_Tags::None))
	{
		EventTag = EventTagOverride;
	}
	else
	{
		const bool bIsDead = Attributes->GetHealth() <= 0.0f;
		EventTag = bIsDead ? CC_Tags::Events::Player::Death : CC_Tags::Events::Player::HitReact;
	}
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, EventTag, Payload);
}

void UCC_BlueprintLibrary::SendDamageEventToPlayers(TArray<AActor*> Targets,
	const TSubclassOf<UGameplayEffect>& DamageEffect, FGameplayEventData& Payload, const FGameplayTag& DataTag,
	float DamageNumber, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	for (AActor* Target : Targets)
	{
		SendDamageEventToPlayer(Target,DamageEffect,Payload,DataTag,DamageNumber,EventTagOverride,OptionalParticleSystem);
	}
}

TArray<AActor*> UCC_BlueprintLibrary::HitBoxOverlapTest(AActor* Target, float HitBoxRadius, float HitBoxForwardOffset,
                                                        float HitBoxElevationOffset, bool bDrawDebugs)
{


	TArray<AActor*> ActorsHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Target);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	FVector Forward = Target->GetActorForwardVector() * HitBoxForwardOffset;
	FVector HitBoxLocation = Target->GetActorLocation() + Forward + FVector(0,0,HitBoxElevationOffset);
	TArray<FOverlapResult> OverlapResults;

	UWorld* World = GEngine->GetWorldFromContextObject(Target,EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return ActorsHit;
	World->OverlapMultiByChannel(OverlapResults,HitBoxLocation,FQuat::Identity,ECC_Visibility,Sphere, QueryParams,ResponseParams);



	for (const FOverlapResult& Result : OverlapResults)
	{
		if (!IsValid(Result.GetActor())) continue;
		ActorsHit.AddUnique(Result.GetActor());
	}

	if (bDrawDebugs)
	{
		DrawDebugSphere(World,HitBoxLocation,HitBoxRadius,16,FColor::Red,false,3.f);
		for (const FOverlapResult& Result : OverlapResults)
		{
			if (IsValid(Result.GetActor()))
			{
				FVector DebugLocation = Result.GetActor()->GetActorLocation();
				DebugLocation.Z += 80.f;
				DrawDebugSphere(World, DebugLocation,50.f,10,FColor::Green,false,3.f);
			}
		}
	}

	return ActorsHit;
}
