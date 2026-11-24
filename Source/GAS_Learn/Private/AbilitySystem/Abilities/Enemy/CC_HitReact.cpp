// Copyright nlmeo


#include "AbilitySystem/Abilities/Enemy/CC_HitReact.h"

#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "GameplayTags/CC_Tags.h"


void UCC_HitReact::CacheHitDirectionVectors(AActor* Instigator)
{
	AvatarForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	const FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector InstigatorLocation = Instigator->GetActorLocation();

	ToInstigator = InstigatorLocation - AvatarLocation;
	ToInstigator.Normalize();

}
