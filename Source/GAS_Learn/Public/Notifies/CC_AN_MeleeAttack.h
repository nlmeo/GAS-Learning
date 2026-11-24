// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CC_AN_MeleeAttack.generated.h"

/**
 *
 */
UCLASS()
class GAS_LEARN_API UCC_AN_MeleeAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;

private:

	TArray<FHitResult> DoSphereTrace(USkeletalMeshComponent* MeshComp) const;

	void SendEventToActor(const TArray<FHitResult>& HitResults, USkeletalMeshComponent* MeshComp) const;

	UPROPERTY(EditAnywhere, Category="Crash|Debugs")
	bool bDrawDebugs = true;

	UPROPERTY(EditAnywhere, Category="Crash|Socket")
	FName SocketName = FName("Muzzle_Front");

	UPROPERTY(EditAnywhere, Category="Crash|Socket")
	float ExtensionOffset = 40;

	UPROPERTY(EditAnywhere, Category="Crash|Socket")
	float SphereTraceRadius = 60;
};
