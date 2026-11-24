#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace CC_Tags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(None);
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Projectile);
	}


	namespace CC_Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActiveOnGiven);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SearchForTarget);

		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack);
		}
	}

	namespace Events
	{
		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);
		}

		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(EndAttack);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeTraceHit);
		}
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(KillScore);
	}
}
