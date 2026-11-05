#include "GameplayTags/CC_Tags.h"

namespace CC_Tags
{
	namespace CC_Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary,"CC_Tags.CC_Abilities.Primary", "Tag for the Primary Gameplay Abilities");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary,"CC_Tags.CC_Abilities.Secondary", "Tag for the Secondary Gameplay Abilities");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary,"CC_Tags.CC_Abilities.Tertiary", "Tag for the Tertiary Gameplay Abilities");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActiveOnGiven,"CC_Tags.CC_Abilities.ActiveOnGiven", "Tag for Abilities that should active immediately once given.");
	}
	namespace Events
	{
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact,"CC_Tags.Events.Enemy.HitReact", "Tag for the Enemy HitReact Event.");
		}
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScore,"CC_Tags.Events.KillScore", "Tag for the KillScore Event.");

	}
}
