// Copyright nlmeo


#include "AbilitySystem/UI/UCC_WidgetComponent.h"

#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "AbilitySystem/UI/CC_AttributeWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/CC_BaseCharacter.h"

void UUCC_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemData();

	if (!IsASCInitialized())
	{
		CrashCharacter->OnASCInitialized.AddDynamic(this,&ThisClass::OnASCInitialized);
	}
	else
	{
		InitializeAttributeDelegate();
	}

}

void UUCC_WidgetComponent::InitAbilitySystemData()
{
	CrashCharacter = Cast<ACC_BaseCharacter>(GetOwner());
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(CrashCharacter->GetAbilitySystemComponent());
	AttributeSet = Cast<UCC_AttributeSet>(CrashCharacter->GetAttributeSet());
}

bool UUCC_WidgetComponent::IsASCInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UUCC_WidgetComponent::InitializeAttributeDelegate()
{
	if (!AttributeSet->bAttributesInitialized)
	{
		AttributeSet->OnAttributeInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);
	}
	else
	{
		BindToAttributeChanges();
	}
}



void UUCC_WidgetComponent::OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(ASC);
	AttributeSet = Cast<UCC_AttributeSet>(AS);

	// Check if the Attribute Set has been initialized with the first GE
	// if not, bind to some delegate that will be broadcast when it is initialized.

	if (!IsASCInitialized()) return;
	InitializeAttributeDelegate();
}

void UUCC_WidgetComponent::BindToAttributeChanges()
{
	
	for (const TTuple<FGameplayAttribute,FGameplayAttribute>& Pair: AttributeMap)
	{
		//如果组件挂在的UI本身就是CC_Attributes或其子类
		BindWidgetToAttributeChanges(GetUserWidgetObject(), Pair);
	
		//如果不是则遍历寻找子类，绑定属性变化事件
		GetUserWidgetObject()->WidgetTree->ForEachWidget([this,&Pair](UWidget* ChildWidget)
		{
			BindWidgetToAttributeChanges(ChildWidget, Pair);
		});
	} 
}

void UUCC_WidgetComponent::BindWidgetToAttributeChanges(UWidget* BindWidget,
	const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	UCC_AttributeWidget* AttributeWidget = Cast<UCC_AttributeWidget>(BindWidget);
	if (!IsValid(AttributeWidget)) return; // Only care about CC_AttributeWidget
	if (!AttributeWidget->MatchesAttributes(Pair)) return; // Only subscribe for matching Attributes
		
	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get()); // for initial values
		
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda([this,AttributeWidget,&Pair](const FOnAttributeChangeData& ChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get()); // For changes during the game.
	});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda([this,AttributeWidget,&Pair](const FOnAttributeChangeData& ChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get()); // For changes during the game.
	});
}