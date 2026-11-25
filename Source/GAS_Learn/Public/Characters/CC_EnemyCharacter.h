// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "CC_BaseCharacter.h"
#include "CC_EnemyCharacter.generated.h"

class UAttributeSet;

UCLASS(BlueprintType)
class GAS_LEARN_API ACC_EnemyCharacter : public ACC_BaseCharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Crash|Aim")
	TObjectPtr<AActor> AimTarget;

	ACC_EnemyCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override{return AttributeSet;};
	virtual void HandleRespawn() override;

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Crash|AI")
	float AcceptRadius = 500.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Crash|AI")
	float MinAttackDelay = 0.1f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Crash|AI")
	float MaxAttackDelay = 0.5f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Crash|AI", Replicated)
	bool bIsBeingLaunched = false;

	UFUNCTION(BlueprintCallable,Category="Crash|AI")
	void StopMovementUntilLanded();

	UFUNCTION(BlueprintCallable,Category="Crash|AI")
	void EnableMovementOnLanded(const FHitResult& HitResult);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Crash")
	float GetTimelineLength();



protected:

	virtual void BeginPlay() override;
	virtual void HandleDeath();
private:
	void UpdateSearchAbility(bool bEnable);
	void RestartSearchAbility();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

};
