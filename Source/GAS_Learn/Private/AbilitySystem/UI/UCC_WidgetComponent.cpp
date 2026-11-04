// Copyright nlmeo


#include "AbilitySystem/UI/UCC_WidgetComponent.h"

#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
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
	// TODO: Listen for changes to Gamaplay Attributes and update our widgets accordingly
}
