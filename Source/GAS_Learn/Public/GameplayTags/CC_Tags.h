#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace CC_Tags
{
	namespace CC_Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActiveOnGiven);
	}

	namespace Events
	{
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
		}
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(KillScore);
	}
}
