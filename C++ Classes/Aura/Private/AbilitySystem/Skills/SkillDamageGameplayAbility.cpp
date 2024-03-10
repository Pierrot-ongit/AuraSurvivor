// Copyright Nono Studios


#include "AbilitySystem/Skills/SkillDamageGameplayAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Skills/SkillTalentTreeData.h"
#include "Aura/AuraLogChannels.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void USkillDamageGameplayAbility::SetupTalentTree()
{
	AAuraPlayerState* PlayerState = Cast<AAuraPlayerState>(OwnerPlayerController->PlayerState);
	TArray<FSkillTalent> SkillTalents = PlayerState->GetTalentsForSkill(AbilityTags.First());
	for (FSkillTalent Talent : SkillTalents)
	{
		TalentsTree.Add(Talent.TalentTag, Talent);
	}
	bTalentTreeSetup = true;
}

float USkillDamageGameplayAbility::GetTalentsModifiersForAttribute(float OutValue, const FGameplayTag& AttributeTag)
{
	if (!bTalentTreeSetup)
	{
		SetupTalentTree();
	}
	
	float AdditiveValue = 0;
	float MultiplicativeCoefficient = 0;
	TArray<FSkillTalent> Talents = FindTalentsForAttribute(AttributeTag);
	if (Talents.Num() > 0)
	{
		for (const FSkillTalent Talent : Talents)
		{
			if (Talent.TalentCondition.ConditionType != ETalentConditionType::None) continue;
			
			if (Talent.TalentType == ETalentType::AttributeAdditive)
			{
				AdditiveValue += Talent.TalentMagnitude * Talent.TalentLevel;
			}
			else if (Talent.TalentType == ETalentType::AttributeMultiplicative)
			{
				MultiplicativeCoefficient += Talent.TalentMagnitude * Talent.TalentLevel;
			}
		}
	}

	OutValue += AdditiveValue;
	if (MultiplicativeCoefficient > 0)
	{
		if (OutValue > 0)
		{
			OutValue *= (1 + MultiplicativeCoefficient / 100);
		}
		else
		{
			OutValue = MultiplicativeCoefficient / 100;
		}
	}
	return OutValue;
}

FDamageEffectParams USkillDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector InKnockbackDirectionOverride,
	bool bOverrideDeathImpulse, FVector InDeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride)
{
	FDamageEffectParams Params =  Super::MakeDamageEffectParamsFromClassDefaults(TargetActor, InRadialDamageOrigin,
	                                                      bOverrideKnockbackDirection, InKnockbackDirectionOverride,
	                                                      bOverrideDeathImpulse, InDeathImpulseDirectionOverride,
	                                                      bOverridePitch, PitchOverride);
	
	if (!bTalentTreeSetup)
	{
		SetupTalentTree();
	}

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	Params.BaseDamage = GetTalentsModifiersForAttribute(Params.BaseDamage, Params.DamageType);
	
	Params.DebuffChance = GetTalentsModifiersForAttribute(Params.DebuffChance, GameplayTags.Debuff_Chance);
	Params.DebuffDamage = GetTalentsModifiersForAttribute(Params.DebuffDamage, GameplayTags.Debuff_Damage);
	Params.DebuffDuration = GetTalentsModifiersForAttribute(Params.DebuffDuration, GameplayTags.Debuff_Duration);
	Params.DebuffFrequency = GetTalentsModifiersForAttribute(Params.DebuffFrequency, GameplayTags.Debuff_Frequency);

	Params.KnockbackChance = GetTalentsModifiersForAttribute(Params.KnockbackChance, GameplayTags.Skills_Attributes_KnockbackChance);
	Params.KnockbackForceMagnitude = GetTalentsModifiersForAttribute(Params.KnockbackChance, GameplayTags.Skills_Attributes_KnockbackForceMagnitude);
	
	Params.SkillCriticalHitChance = GetTalentsModifiersForAttribute(0, GameplayTags.Skills_Attributes_CriticalHitChance);
	Params.SkillCriticalHitDamage = GetTalentsModifiersForAttribute(0, GameplayTags.Skills_Attributes_CriticalHitDamage);
	Params.SkillArmorPenetration = GetTalentsModifiersForAttribute(0, GameplayTags.Skills_Attributes_ArmorPenetration);
	Params.SkillTag = AbilityTags.First();
	
	return Params;
}

const FTalentData* USkillDamageGameplayAbility::FindTalentDataForTag(const FGameplayTag& TalentTag, bool bLogNotFound) const
{
	return SkillTalentTree->GetTalentData(TalentTag);
}

TArray<FSkillTalent> USkillDamageGameplayAbility::FindTalentsForAttribute(const FGameplayTag& AttributeTag) const
{
	TArray<FSkillTalent> TalentsAttributes;
	for (const TTuple<FGameplayTag, FSkillTalent> Pair : TalentsTree)
	{
		if (Pair.Value.AttributeTag == AttributeTag)
		{
			UE_LOG(LogAura, Display, TEXT("Attribute find for TalentTag [%s] for attribute [%s]"), *Pair.Value.TalentTag.ToString(), *Pair.Value.AttributeTag.ToString());	

			TalentsAttributes.Add(Pair.Value);
		}
	}

	return TalentsAttributes;
}

TArray<FTalentData> USkillDamageGameplayAbility::GetTalentsTree()
{
	return SkillTalentTree->TalentsInformations;
}
