#include "GameplayTags/CC_Tags.h"

namespace CC_Tags
{

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(None,"CC_Tags.None", "None");
	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Projectile,"CC_Tags.SetByCaller.Projectile", "Tag for Set by Caller Magnitude for Projectile");
	}

	namespace CC_Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary,"CC_Tags.CC_Abilities.Primary", "Tag for the Primary Gameplay Abilities");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary,"CC_Tags.CC_Abilities.Secondary", "Tag for the Secondary Gameplay Abilities");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary,"CC_Tags.CC_Abilities.Tertiary", "Tag for the Tertiary Gameplay Abilities");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActiveOnGiven,"CC_Tags.CC_Abilities.ActiveOnGiven", "Tag for Abilities that should active immediately once given.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SearchForTarget,"CC_Tags.CC_Abilities.SearchForTarget", "Tag for the Search For Target Gameplay Ability.");

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack,"CC_Tags.CC_Abilities.Enemy.Attack", "Tag for the Enemy Attack Gameplay Abilities");
		}
	}
	namespace Events
	{

		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact,"CC_Tags.Events.Player.HitReact", "Tag for the Player HitReact Event.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death,"CC_Tags.Events.Player.Death", "Tag for the Player Death Event.");
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact,"CC_Tags.Events.Enemy.HitReact", "Tag for the Enemy HitReact Event.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(EndAttack,"CC_Tags.Events.Enemy.EndAttack", "Tag for the Enemy EndAttack Event.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeTraceHit,"CC_Tags.Events.Enemy.MeleeTraceHit", "Tag for the Enemy Melee Trace Hit Event.");


		}
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScore,"CC_Tags.Events.KillScore", "Tag for the KillScore Event.");

	}
}
