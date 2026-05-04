// Fill out your copyright notice in the Description page of Project Settings.

#include "CryptRaiderGameplayTags.h"

namespace CryptRaiderGameplayTags
{
	/** Player Tags **/
	
		/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ToggleMenu, "InputTag.ToggleMenu")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_ToggleWeapon, "InputTag.ToggleWeapon")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Dodge, "InputTag.Dodge")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Heal, "InputTag.Heal")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Equip_Weapon_Light, "InputTag.Equip.Weapon.Light")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Equip_Weapon_Heavy, "InputTag.Equip.Weapon.Heavy")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Unequip, "InputTag.Unequip")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LightAttack_Axe, "InputTag.LightAttack_Axe")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HeavyAttack_Axe, "InputTag.HeavyAttack_Axe")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_LightAttack_Sword, "InputTag.LightAttack_Sword")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HeavyAttack_Sword, "InputTag.HeavyAttack_Sword")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SwitchTarget, "InputTag.SwitchTarget")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Light, "InputTag.SpecialWeaponAbility.Light")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_SpecialWeaponAbility_Heavy, "InputTag.SpecialWeaponAbility.Heavy")
	

	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable, "InputTag.Toggleable")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_TargetLock, "InputTag.Toggleable.TargetLock")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Toggleable_Magic, "InputTag.Toggleable.Magic")
	
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld, "InputTag.MustBeHeld")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MustBeHeld_Block, "InputTag.MustBeHeld.Block")	
		/** Abilities **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip, "Player.Ability.Equip")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip, "Player.Ability.Unequip")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Interact, "Player.Ability.Interact")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Block, "Player.Ability.Block")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_TargetLock, "Player.Ability.TargetLock")
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Light_Attack_Axe, "Player.Ability.Attack.Light.Axe")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Heavy_Attack_Axe, "Player.Ability.Attack.Heavy.Axe")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Light_Axe, "Player.Ability.SpecialWeaponAbility.Light.Axe")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Heavy_Axe, "Player.Ability.SpecialWeaponAbility.Heavy.Axe")
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Light_Sword, "Player.Ability.Attack.Light.Sword")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Heavy_Sword, "Player.Ability.Attack.Heavy.Sword")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Light_Sword, "Player.Ability.SpecialWeaponAbility.Light.Sword")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_SpecialWeaponAbility_Heavy_Sword, "Player.Ability.SpecialWeaponAbility.Heavy.Sword")
	
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_HitPause, "Player.Ability.HitPause")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Dodge, "Player.Ability.Dodge")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Magic, "Player.Ability.Magic")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_RecoverStamina, "Player.Ability.RecoverStamina")

		/** Events **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_EquipLightWeapon, "Player.Event.EquipLightWeapon")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_UnequipLightWeapon, "Player.Event.UnequipLightWeapon")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_EquipHeavyWeapon, "Player.Event.EquipHeavyWeapon")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_UnequipHeavyWeapon, "Player.Event.UnequipHeavyWeapon")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwapWeapon, "Player.Event.SwapWeapon")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip, "Player.Event.Equip")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip, "Player.Event.Unequip")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_HitPause, "Player.Event.HitPause")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SuccessfulBlock, "Player.Event.SuccessfulBlock")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Left, "Player.Event.SwitchTarget.Left")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_SwitchTarget_Right, "Player.Event.SwitchTarget.Right")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_ActivateMagic, "Player.Event.ActivateMagic")
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_UseStamina, "Player.Event.UseStamina")


		/** Statuses **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToNextAttack, "Player.Status.JumpToNextAttack")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToFinisher, "Player.Status.JumpToFinisher")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Dodging, "Player.Status.Dodging")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Blocking, "Player.Status.Blocking")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Attacking, "Player.Status.Attacking")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Inspecting, "Player.Status.Inspecting")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_TargetLock, "Player.Status.TargetLock")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Stamina_None,  "Player.Status.Stamina.None");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Stamina_NotFull,  "Player.Status.Stamina.NotFull");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Stamina_Full,  "Player.Status.Stamina.Full");
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Magic_Activating, "Player.Status.Magic.Activating")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Magic_Active, "Player.Status.Magic.Active")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Magic_Full, "Player.Status.Magic.Full")
	UE_DEFINE_GAMEPLAY_TAG(Player_Status_Magic_None, "Player.Status.Magic.None")

	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_StaminaRegen, "Player.Cooldown.StaminaRegen")
	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_SpecialWeaponAbility_Light, "Player.Cooldown.SpecialWeaponAbility.Light")
	UE_DEFINE_GAMEPLAY_TAG(Player_Cooldown_SpecialWeaponAbility_Heavy, "Player.Cooldown.SpecialWeaponAbility.Heavy")
	
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Light, "Player.SetByCaller.AttackType.Light")
	UE_DEFINE_GAMEPLAY_TAG(Player_SetByCaller_AttackType_Heavy, "Player.SetByCaller.AttackType.Heavy")

	/** Enemy Tags **/

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon, "Enemy.Weapon")
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Melee, "Enemy.Ability.Melee")
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Ranged, "Enemy.Ability.Ranged")
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_Dodge, "Enemy.Ability.Dodge")
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Ability_SummonEnemies, "Enemy.Ability.SummonEnemies")
	
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Strafing, "Enemy.Status.Strafing")
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_UnderAttack, "Enemy.Status.UnderAttack")
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Status_Patrolling, "Enemy.Status.Patrolling")

	UE_DEFINE_GAMEPLAY_TAG(Enemy_Event_SummonEnemies, "Enemy.Event.SummonEnemies")
	
	/** Shared Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit, "Shared.Event.MeleeHit")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_HitReact, "Shared.Event.HitReact")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_SpawnProjectile, "Shared.Event.SpawnProjectile")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Event_AOE, "Shared.Event.AOE")
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_HitReact, "Shared.Ability.HitReact")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Ability_Death, "Shared.Ability.Death")
	
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage, "Shared.SetByCaller.BaseDamage")
	UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_SpecialDamage, "Shared.SetByCaller.SpecialDamage")

	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Dead, "Shared.Status.Dead")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Front, "Shared.Status.HitReact.Front")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Left, "Shared.Status.HitReact.Left")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Right, "Shared.Status.HitReact.Right")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_HitReact_Back, "Shared.Status.HitReact.Back")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_BuffedAttack, "Shared.Status.BuffedAttack")
	UE_DEFINE_GAMEPLAY_TAG(Shared_Status_Invincible, "Shared.Status.Invincible")
}
