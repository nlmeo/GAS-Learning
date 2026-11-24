// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTags/CC_Tags.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CC_BlueprintLibrary.generated.h"


UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Left,
	Right,
	Forward,
	Back
};


USTRUCT(BlueprintType)
struct FClosestActorWithTagResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float Distance = 0.f;

	FClosestActorWithTagResult() = default;
	FClosestActorWithTagResult(AActor* Actor,const float& Distance) : Actor(Actor), Distance(Distance) {}

};

UCLASS(BlueprintType)
class GAS_LEARN_API UCC_BlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure)
	static EHitDirection GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator);

	UFUNCTION(BlueprintPure)
	static FName GetHitDirectionName(const EHitDirection& Direction);

	UFUNCTION(BlueprintCallable)
	static FClosestActorWithTagResult FindClosestActorWithTag(const UObject* WorldContextObject,const FVector& Origin, const FName& Tag, float SearchRange = 1000.f);

	UFUNCTION(BlueprintCallable)
	static void SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect>& DamageEffect,UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag, float DamageNumber,const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem = nullptr);

	UFUNCTION(BlueprintCallable)
	static void SendDamageEventToPlayers(TArray<AActor*> Targets, const TSubclassOf<UGameplayEffect>& DamageEffect,UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag, float DamageNumber,const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem = nullptr);


	UFUNCTION(BlueprintCallable)
	static TArray<AActor*> HitBoxOverlapTest(AActor* Target, float HitBoxRadius, float HitBoxForwardOffset, float HitBoxElevationOffset,bool bDrawDebugs = false);


};
