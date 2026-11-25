#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Characters/CC_EnemyCharacter.h"
#include "Animation/AnimInstance.h"
#include "UObject/UnrealType.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameObjects/CC_Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/TimelineComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCC_RangedAimTest, "GAS_Learn.Combat.RangedHeightAim",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCC_RangedAimTest::RunTest(const FString& Parameters)
{
	UClass* EnemyClass = LoadClass<ACC_EnemyCharacter>(nullptr, TEXT("/Game/GAS_Learn/Characters/EnemyCharacter/BP_Enemy_Ranged.BP_Enemy_Ranged_C"));
	UClass* ProjectileClass = LoadClass<ACC_Projectile>(nullptr, TEXT("/Game/GAS_Learn/GameObjects/BP_CC_Projectile.BP_CC_Projectile_C"));
	if (!TestNotNull(TEXT("Ranged blueprint"), EnemyClass) || !TestNotNull(TEXT("Projectile blueprint"), ProjectileClass)) return false;
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	ACC_EnemyCharacter* Enemy = World->SpawnActor<ACC_EnemyCharacter>(EnemyClass);
	ACC_EnemyCharacter* Target = World->SpawnActor<ACC_EnemyCharacter>(ACC_EnemyCharacter::StaticClass(), FVector(500, 0, 300), FRotator::ZeroRotator);
	if (!Enemy || !Target) { AddError(TEXT("Failed to spawn test actors")); World->DestroyWorld(false); return false; }
	Enemy->AimTarget = Target;
	USkeletalMeshComponent* Mesh = Enemy->GetMesh();
	Mesh->InitAnim(true);
	UAnimInstance* Anim = Mesh->GetAnimInstance();
	if (!TestNotNull(TEXT("Ranged animation instance"), Anim)) { World->DestroyWorld(false); return false; }
	TestEqual(TEXT("Animation blueprint uses standard AnimInstance parent"), Anim->GetClass()->GetSuperClass(), UAnimInstance::StaticClass());
	const FStructProperty* AimProperty = FindFProperty<FStructProperty>(Anim->GetClass(), TEXT("AimRotation"));
	if (!TestNotNull(TEXT("Blueprint aim rotation"), AimProperty)) { World->DestroyWorld(false); return false; }
	auto AimRotation = [&]() { return *AimProperty->ContainerPtrToValuePtr<FRotator>(Anim); };
	Enemy->AimTarget = nullptr;
	Mesh->TickAnimation(1.f, false);
	Mesh->RefreshBoneTransforms();
	const float NeutralMuzzlePitch = Mesh->GetSocketRotation(TEXT("Muzzle_Front")).Pitch;
	const FTransform NeutralLeftFoot = Mesh->GetSocketTransform(TEXT("foot_l"), RTS_World);
	const FTransform NeutralRightFoot = Mesh->GetSocketTransform(TEXT("foot_r"), RTS_World);
	Enemy->AimTarget = Target;
	for (float Height : {300.f, -300.f, 0.f})
	{
		Target->SetActorLocation(Enemy->GetActorLocation() + FVector(500, 0, Height));
		// Exercise the actual blueprint turning timeline, which previously pitched the whole pawn.
		Enemy->RotateToTarget(Target);
		TArray<UTimelineComponent*> Timelines;
		Enemy->GetComponents(Timelines);
		TestTrue(TEXT("Enemy turn timeline exists"), !Timelines.IsEmpty());
		for (int32 Step = 0; Step < 40; ++Step)
		{
			for (UTimelineComponent* Timeline : Timelines)
				Timeline->TickComponent(0.05f, LEVELTICK_All, nullptr);
			TestTrue(TEXT("Turning never pitches or rolls the whole character"),
				FMath::IsNearlyZero(Enemy->GetActorRotation().Pitch) && FMath::IsNearlyZero(Enemy->GetActorRotation().Roll));
		}
		Anim->BlueprintUpdateAnimation(1.f);
		const FQuat MeshRotation = Mesh->GetComponentQuat();
		const FQuat WorldAim = MeshRotation * AimRotation().Quaternion() * MeshRotation.Inverse();
		const FVector Aim = WorldAim.RotateVector(Enemy->GetActorForwardVector());
		const FVector Expected = (Target->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
		TestTrue(TEXT("Upper body aims toward target elevation"), FVector::DotProduct(Aim, Expected) > 0.999f);
		TestEqual(TEXT("Character capsule stays upright"), Enemy->GetActorRotation().Pitch, 0.0);
		Mesh->TickAnimation(0.f, false);
		Mesh->RefreshBoneTransforms();
		TestTrue(TEXT("Left foot keeps original standing pose"), Mesh->GetSocketTransform(TEXT("foot_l"), RTS_World).Equals(NeutralLeftFoot, 0.1f));
		TestTrue(TEXT("Right foot keeps original standing pose"), Mesh->GetSocketTransform(TEXT("foot_r"), RTS_World).Equals(NeutralRightFoot, 0.1f));
		const float MuzzlePitchDelta = FMath::FindDeltaAngleDegrees(NeutralMuzzlePitch,
			static_cast<float>(Mesh->GetSocketRotation(TEXT("Muzzle_Front")).Pitch));
		if (Height > 0.f) TestTrue(FString::Printf(TEXT("Actual muzzle tilts up (%.2f degrees)"), MuzzlePitchDelta), MuzzlePitchDelta > 10.f);
		if (Height < 0.f) TestTrue(FString::Printf(TEXT("Actual muzzle tilts down (%.2f degrees)"), MuzzlePitchDelta), MuzzlePitchDelta < -10.f);
		const FTransform SpawnTransform(FRotator::ZeroRotator, Enemy->GetActorLocation() + FVector(0, 0, 50));
		ACC_Projectile* Shot = World->SpawnActorDeferred<ACC_Projectile>(ProjectileClass, SpawnTransform, Enemy, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Shot->Shooter = Enemy;
		Shot->FinishSpawning(SpawnTransform);
		// This isolated world does not BeginPlay automatically.
		Shot->DispatchBeginPlay();
		const auto* Movement = Shot->FindComponentByClass<UProjectileMovementComponent>();
		const FVector ExpectedShot = (Target->GetActorLocation() - Shot->GetActorLocation()).GetSafeNormal();
		TestTrue(TEXT("Projectile has nonzero launch speed"), !Movement->Velocity.IsNearlyZero());
		TestTrue(TEXT("Projectile launches toward target elevation"), FVector::DotProduct(Movement->Velocity.GetSafeNormal(), ExpectedShot) > 0.999f);
		Shot->Destroy();
	}
	Enemy->AimTarget = nullptr;
	Anim->BlueprintUpdateAnimation(1.f);
	TestTrue(TEXT("Lost target restores neutral aim"), AimRotation().IsNearlyZero());
	World->DestroyWorld(false);
	return true;
}
#endif
