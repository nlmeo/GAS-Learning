// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_Primary.generated.h"


UCLASS()
class GAS_LEARN_API UCC_Primary : public UCC_GameplayAbility
{
	GENERATED_BODY()
public:

	//UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	//TArray<AActor*> HitBoxOverlapTest();

	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void SendHitReactEventToActors(const TArray<AActor*>& HitActors);
protected:

	//void DrawHitBoxOverlapDebugs( const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Crash|Abilities")
	float HitBoxRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "Crash|Abilities")
	float HitBoxForwardOffset = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "Crash|Abilities")
	float HitBoxElevationOffset = 20.f;
};
