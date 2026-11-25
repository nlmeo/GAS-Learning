// Copyright nlmeo

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Characters/CC_PlayerCharacter.h"
#include "Engine/World.h"
#include "Player/CC_PlayerState.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCC_PlayerRespawnTest,
	"GAS_Learn.Characters.PlayerRespawnResetsAttributes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCC_PlayerRespawnTest::RunTest(const FString& Parameters)
{
	UClass* PlayerClass = LoadClass<ACC_PlayerCharacter>(nullptr,
		TEXT("/Game/GAS_Learn/Characters/PlayerCharacter/BP_CC_PlayerCharacter.BP_CC_PlayerCharacter_C"));
	if (!TestNotNull(TEXT("Player blueprint"), PlayerClass)) return false;

	UWorld::InitializationValues WorldSettings;
	WorldSettings
		.AllowAudioPlayback(false).CreatePhysicsScene(true)
		.CreateNavigation(false).CreateAISystem(false);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
		true, ERHIFeatureLevel::Num, &WorldSettings);
	ACC_PlayerCharacter* Player = World->SpawnActor<ACC_PlayerCharacter>(PlayerClass);
	ACC_PlayerState* State = World->SpawnActor<ACC_PlayerState>();
	if (!TestNotNull(TEXT("Player"), Player) || !TestNotNull(TEXT("PlayerState"), State))
	{
		World->DestroyWorld(false);
		return false;
	}

	Player->SetPlayerState(State);
	UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(State, Player);
	UCC_AttributeSet* Attributes = CastChecked<UCC_AttributeSet>(Player->GetAttributeSet());
	// This isolated world never enters BeginPlay, so register the set explicitly.
	ASC->AddAttributeSetSubobject(Attributes);
	ASC->SetNumericAttributeBase(UCC_AttributeSet::GetMaxHealthAttribute(), 250.f);
	ASC->SetNumericAttributeBase(UCC_AttributeSet::GetMaxManaAttribute(), 120.f);

	// Reuse the PlayerState ASC across two deaths, as the player respawn flow does.
	for (int32 DeathIndex = 0; DeathIndex < 2; ++DeathIndex)
	{
		ASC->SetNumericAttributeBase(UCC_AttributeSet::GetHealthAttribute(), 0.f);
		ASC->SetNumericAttributeBase(UCC_AttributeSet::GetManaAttribute(), 0.f);
		Player->SetAlive(false);
		Player->HandleRespawn();
		TestTrue(TEXT("Respawn restores alive state"), Player->IsAlive());
		TestEqual(TEXT("Respawn restores health"), Attributes->GetHealth(), Attributes->GetMaxHealth());
		TestEqual(TEXT("Respawn restores mana"), Attributes->GetMana(), Attributes->GetMaxMana());

		ASC->SetNumericAttributeBase(UCC_AttributeSet::GetHealthAttribute(), 80.f);
		ASC->SetNumericAttributeBase(UCC_AttributeSet::GetManaAttribute(), 30.f);
		Player->HandleRespawn();
		TestEqual(TEXT("Duplicate callback preserves health"), Attributes->GetHealth(), 80.f);
		TestEqual(TEXT("Duplicate callback preserves mana"), Attributes->GetMana(), 30.f);
	}

	World->DestroyWorld(false);
	return true;
}

#endif
