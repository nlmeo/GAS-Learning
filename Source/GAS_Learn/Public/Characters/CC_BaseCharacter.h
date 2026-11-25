#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "GameFramework/Character.h"

#include "CC_BaseCharacter.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FASCInitialized,UAbilitySystemComponent*, ASC, UAttributeSet*, AS);


namespace CrashTags
{
	extern GAS_LEARN_API const FName PlayerTag;
	extern GAS_LEARN_API const FName EnemyTag;
}


UCLASS(Abstract)
class GAS_LEARN_API ACC_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACC_BaseCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const {return nullptr;};
	bool IsAlive() const {return bAlive;}
	void SetAlive(bool bAliveStatus) {bAlive = bAliveStatus;}

	UPROPERTY(BlueprintAssignable)
	FASCInitialized OnASCInitialized;

	UFUNCTION(BlueprintCallable, Category = "Crash|Death")
	virtual void HandleRespawn();

	UFUNCTION(BlueprintCallable, Category = "Crash|Reset")
	virtual void ResetAttributes() const;

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Crash")
	void RotateToTarget(AActor* TargetActor);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Crash|AI")
	float SearchRange = 1000.f;
protected:
	void GiveStartupAbilities();
	void InitializeAttributes() const;

	void OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData);
	virtual void HandleDeath();
private:
	// 客户端死亡技能的动画回调需要将重生交给服务器完成。
	UFUNCTION(Server, Reliable)
	void ServerHandleRespawn();

	UPROPERTY(EditdefaultsOnly, Category="Crash|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditdefaultsOnly, Category="Crash|Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

	UPROPERTY(EditdefaultsOnly, Category="Crash|Effects")
	TSubclassOf<UGameplayEffect> ResetAttributesEffect;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bAlive = true;
};
