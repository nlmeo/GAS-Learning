// Copyright nlmeo

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "CC_Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class GAS_LEARN_API ACC_Projectile : public AActor
{
	GENERATED_BODY()

public:
	ACC_Projectile();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Crash|Damage", meta=(ExposeOnSpawn, ClampMin="0.0"))
	float Damage{-25.f};

	// 在 Spawn Actor 时传入发射者，供命中时确定伤害来源。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crash|Projectile", meta=(ExposeOnSpawn="true"))
	TObjectPtr<AActor> Shooter = nullptr;

	UFUNCTION(BlueprintPure, Category="Crash|Projectile")
	AActor* GetShooter() const { return IsValid(Shooter.Get()) ? Shooter.Get() : nullptr; }

	UFUNCTION(BlueprintImplementableEvent, Category="Crash|Projectile")
	void SpawnImpactEffects();
private:

	// 服务器调用；服务器和相关客户端各执行一次本地的蓝图特效事件。
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnImpactEffects(const FTransform& ImpactTransform);

	// 防止同一发投射物重复结算碰撞和发送特效。
	bool bImpactHandled{false};

	UPROPERTY(VisibleAnywhere, Category="Crash|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category="Crash|Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;
};
