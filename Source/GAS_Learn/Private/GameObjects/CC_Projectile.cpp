// Copyright nlmeo


#include "GameObjects/CC_Projectile.h"

#include "Characters/CC_PlayerCharacter.h"
#include "Characters/CC_EnemyCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayTags/CC_Tags.h"
#include "Utils/CC_BlueprintLibrary.h"


ACC_Projectile::ACC_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
}

void ACC_Projectile::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	const ACC_EnemyCharacter* Enemy = Cast<ACC_EnemyCharacter>(Shooter);
	if (!IsValid(Enemy) || !IsValid(Enemy->AimTarget)) return;
	const ACC_BaseCharacter* Target = Cast<ACC_BaseCharacter>(Enemy->AimTarget);
	if (Target && !Target->IsAlive()) return;
	// 瞄准角色中心，保留高度差；只修正初始方向，不做追踪弹。
	const FVector Direction = (Enemy->AimTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	if (Direction.IsNearlyZero()) return;
	const float Speed = ProjectileMovement->Velocity.Size() > UE_SMALL_NUMBER
		? ProjectileMovement->Velocity.Size() : ProjectileMovement->InitialSpeed;
	SetActorRotation(Direction.Rotation());
	ProjectileMovement->Velocity = Direction * Speed;
	ForceNetUpdate();
}

void ACC_Projectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!HasAuthority() || bImpactHandled) return;

	ACC_PlayerCharacter* PlayerCharacter = Cast<ACC_PlayerCharacter>(OtherActor);
	if (!IsValid(PlayerCharacter)) return;
	if (!PlayerCharacter->IsAlive()) return;
	if (!IsValid(Shooter)) return;

	bImpactHandled = true;

	FGameplayEventData Playload;
	// 发射者作为伤害来源；引用失效时，GC 的安全方向函数会回退到 Front。
	Playload.Instigator = Shooter;
	Playload.Target = PlayerCharacter;

	UCC_BlueprintLibrary::SendDamageEventToPlayer
	(
		OtherActor,
		DamageEffect,
		Playload,
		CC_Tags::SetByCaller::Projectile,
		Damage,
		CC_Tags::None

	);

	const FTransform ImpactTransform = GetActorTransform();
	// 在销毁前发送可靠多播；服务器也会通过多播执行本地特效，避免重复生成。
	MulticastSpawnImpactEffects(ImpactTransform);

	Destroy();
}

void ACC_Projectile::MulticastSpawnImpactEffects_Implementation(const FTransform& ImpactTransform)
{
	bImpactHandled = true;
	SetActorEnableCollision(false);
	ProjectileMovement->StopMovementImmediately();
	// 使用服务器命中时的位置，使蓝图中的特效挂点不受客户端移动同步误差影响。
	SetActorTransform(ImpactTransform, false, nullptr, ETeleportType::TeleportPhysics);
	if (GetNetMode() != NM_DedicatedServer)
	{
		SpawnImpactEffects();
	}
}
