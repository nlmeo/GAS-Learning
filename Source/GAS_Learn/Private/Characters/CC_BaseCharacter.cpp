#include "GAS_Learn/Public/Characters/CC_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"


ACC_BaseCharacter::ACC_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Tick and refresh bone transforms whether render or not - for bone updates on a dedicated server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

}

UAbilitySystemComponent* ACC_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}


void ACC_BaseCharacter::GiveStartupAbilities()
{
	if (!IsValid(GetAbilitySystemComponent())) return;

	for (const auto& Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
}

// 将配置好的 GameplayEffect 应用到自身，由效果中的 Modifiers 设置 AttributeSet 的初始属性。
// 调用前需要确保 ASC（AbilitySystemComponent）有效，且已完成 InitAbilityActorInfo。
void ACC_BaseCharacter::InitializeAttributes() const
{
	// 检查是否配置了用于初始化属性的 GameplayEffect 类。
	// checkf 是开发期断言：检查失败时报告错误；它不能代替 Shipping 版本中的运行时保护。
	checkf(IsValid(InitializeAttributesEffect),TEXT("InitializeAttributesEffect not set."));

	// 创建效果上下文，用来携带效果的来源、发起者等信息。
	// MakeEffectContext 会根据 ASC 的 ActorInfo 填入默认信息，也可以按需补充命中结果等数据。
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	// 根据效果类创建本次应用的效果规格（Spec），保存等级、上下文等运行时数据。
	// 1.f 表示效果等级为 1；若效果数值使用等级缩放，这个等级会参与数值计算。
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);

	// 取出句柄中保存的 Spec，并解引用后传给 ASC，将效果应用到自身。
	// ASC 会根据效果配置修改已注册的 AttributeSet；具体属性和数值由效果资产决定。
	// 这里假定 Spec 创建成功，SpecHandle.Data 有效。
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ACC_BaseCharacter::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	if (AttributeChangeData.NewValue <= 0.f)
	{
		HandleDeath();
	}
}

void ACC_BaseCharacter::HandleDeath()
{
	bAlive = false;
}

void ACC_BaseCharacter::HandleRespawn()
{
	bAlive = true;	
}

