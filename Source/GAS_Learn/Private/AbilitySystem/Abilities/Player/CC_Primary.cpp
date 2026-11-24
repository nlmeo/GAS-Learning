// Copyright nlmeo


#include "AbilitySystem/Abilities/Player/CC_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/CC_Tags.h"



void UCC_Primary::SendHitReactEventToActors(const TArray<AActor*>& HitActors)
{
	for (AActor* HitActor : HitActors)
	{
		if(!IsValid(HitActor)) continue;

		FGameplayEventData PayLoad;
		PayLoad.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, CC_Tags::Events::Enemy::HitReact,PayLoad);
	}
}

/**
TArray<AActor*> UCC_Primary::HitBoxOverlapTest()
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitBoxForwardOffset;
	FVector HitBoxLocation = GetAvatarActorFromActorInfo()->GetActorLocation() + Forward + FVector(0,0,HitBoxElevationOffset);
	TArray<FOverlapResult> OverlapResults;

	GetWorld()->OverlapMultiByChannel(OverlapResults,HitBoxLocation,FQuat::Identity,ECC_Visibility,Sphere, QueryParams,ResponseParams);


	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (!IsValid(Result.GetActor())) continue;
		ActorsHit.AddUnique(Result.GetActor());
	}

	if (bDrawDebugs)
	{
		DrawHitBoxOverlapDebugs(OverlapResults, HitBoxLocation);
	}

	return ActorsHit;

}

void UCC_Primary::DrawHitBoxOverlapDebugs(const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation) const
{
	DrawDebugSphere(GetWorld(),HitBoxLocation,HitBoxRadius,16,FColor::Red,false,3.f);
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 80.f;
			DrawDebugSphere(GetWorld(),DebugLocation,50.f,10,FColor::Green,false,3.f);
		}
	}
}
*/
