// Copyright Nono Studios


#include "AbilitySystem/ExecCalc/Skills_ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

// Just a raw internal struct, not used anywhere else, not in blueprint etc. So not a USTRUCT, and no prefixing with an F.
struct SkillsAuraDamageStatics
{
	// With 'Armor' as parameter, it will create a FGameplayEffectAttributeCaptureDefinition ArmorDef
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	
	SkillsAuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, MaxHealth, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false); // Source not Target, pay attention to it.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
	}
};

static const SkillsAuraDamageStatics& SkillDamageStatics()
{
	// When you create a static variable here inside of a static function, then every time that function is called,
	// we get that same object. It has static storage duration. It doesn't go away when the function ends, even though
	// it's not dynamically allocated on the heap as a pointer using the new keyword, it doesn't matter because it's static,
	// it's going to live on.
	static SkillsAuraDamageStatics DStatics;
	return DStatics;
}

USkills_ExecCalc_Damage::USkills_ExecCalc_Damage()
{
	// ArmorDef created with macro.
	RelevantAttributesToCapture.Add(SkillDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().PhysicalResistanceDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(SkillDamageStatics().MaxHealthDef);
}

void USkills_ExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              const FGameplayEffectSpec& Spec,
                                              FAggregatorEvaluateParameters EvaluationParameters,
                                              const TMap<FGameplayTag,
                                              FGameplayEffectAttributeCaptureDefinition>& InTagsToDef,
                                              TArray<FSkillTalent> SkillTalents) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& DebuffTag = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, -1.f);
		if (TypeDamage > -1.f)
		{
			// Determine if there was a successful debuff.
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1);
			float TargetDebuffResistance = 0.f;
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageTypeTag];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDef[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
			TargetDebuffResistance = FMath::Max<float>(0.f, TargetDebuffResistance);
			const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;
			const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
			if (bDebuff)
			{
				FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
				UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(EffectContextHandle, bDebuff);
				UAuraAbilitySystemLibrary::SetDamageType(EffectContextHandle, DamageTypeTag);
				
				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);
				UAuraAbilitySystemLibrary::SetDebuffDamage(EffectContextHandle, DebuffDamage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(EffectContextHandle, DebuffDuration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(EffectContextHandle, DebuffFrequency);
				
				// Another way could (may)be with delegate when the debuff is applied inside AuraAttributeSet.
				for (FSkillTalent SkillTalent : SkillTalents)
				{
					if (SkillTalent.TalentCondition.ConditionType == ETalentConditionType::TargetReceiveTag
						&& SkillTalent.TalentCondition.ConditionTag == DebuffTag
						&& IsValid(SkillTalent.TalentEffectClass))
					{
						TSubclassOf<UGameplayEffect> GameplayEffectClass = SkillTalent.TalentEffectClass;
						UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
						FGameplayEffectContextHandle TalentEffectContextHandle = TargetASC->MakeEffectContext();
						TalentEffectContextHandle.AddSourceObject(TargetASC->GetAvatarActor());
						FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, SkillTalent.TalentLevel, TalentEffectContextHandle);
						// TODO Link the duration of the TalentDebuff GameplayEffect (ex: Pyrophobia) with the duration of the original debuff (like burn).
						// Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
						// Effect->Period = DebuffFrequency;
						// Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
						TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
					}
				}
			}
		}
	}
}


void USkills_ExecCalc_Damage::HandleTalentConditionAttribute(float& OutValue, FSkillTalent SkillTalent,
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs,
	TMap<FGameplayTag, FGameplayTag> TagsCurrentAttributeToMaxAttribute,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters EvaluationParameters,
	bool ConditionAttributeAboveThresold) const
{
	const FGameplayTag& ConditionAttribute = SkillTalent.TalentCondition.ConditionAttributeTag;
	if (TagsToCaptureDefs.Contains(ConditionAttribute))
	{
		const FGameplayEffectAttributeCaptureDefinition CaptureDefCondition = TagsToCaptureDefs[ConditionAttribute];
		float AttributeValue = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDefCondition, EvaluationParameters, AttributeValue);

		if (SkillTalent.TalentCondition.AttributeValueInPercent)
		{
			if (TagsCurrentAttributeToMaxAttribute.Contains(SkillTalent.TalentCondition.ConditionAttributeTag))
			{
				const FGameplayEffectAttributeCaptureDefinition CaptureDefMaxCondition = TagsToCaptureDefs[TagsCurrentAttributeToMaxAttribute[ConditionAttribute]];
				float MaxAttributeValue = 0.f;
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDefMaxCondition, EvaluationParameters, MaxAttributeValue);
				const float Percent = (AttributeValue / MaxAttributeValue) * 100;
				if (ConditionAttributeAboveThresold)
				{
					if (Percent > SkillTalent.TalentCondition.AttributeValue)
					{
						ChangeValueByTalentModifier(OutValue, SkillTalent);
					}
				}
				else
				{
					if (Percent < SkillTalent.TalentCondition.AttributeValue)
					{
						ChangeValueByTalentModifier(OutValue, SkillTalent);
					}
				}
			}
		}
		else
		{
			if (ConditionAttributeAboveThresold)
			{
				if (AttributeValue > SkillTalent.TalentCondition.AttributeValue)
				{
					ChangeValueByTalentModifier(OutValue, SkillTalent);
				}
			}
			else
			{
				if (AttributeValue < SkillTalent.TalentCondition.AttributeValue)
				{
					ChangeValueByTalentModifier(OutValue, SkillTalent);
				}
			}
		}
	}
}

void USkills_ExecCalc_Damage::HandleTalentsModifications(float& OutValue, const FGameplayTag& ValueAttribute,
                                                         const TArray<FSkillTalent> SkillTalents,
                                                         TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs,
                                                         TMap<FGameplayTag, FGameplayTag> TagsCurrentAttributeToMaxAttribute,
                                                         const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters EvaluationParameters,
                                                         const UAbilitySystemComponent* SourceASC, const UAbilitySystemComponent* TargetASC) const
{
	for (FSkillTalent SkillTalent : SkillTalents)
	{
		if (!SkillTalent.AttributeTag.MatchesTag(ValueAttribute)) continue;

		if (SkillTalent.TalentCondition.ConditionType == ETalentConditionType::None)
		{
			ChangeValueByTalentModifier(OutValue, SkillTalent);
			continue;
		}

		switch (SkillTalent.TalentCondition.ConditionType)
		{
			// Player.
		case ETalentConditionType::PlayerAttributeBelow:
			HandleTalentConditionAttribute(OutValue, SkillTalent, TagsToCaptureDefs, TagsCurrentAttributeToMaxAttribute, ExecutionParams,
			                               EvaluationParameters, false);
			break;

		case ETalentConditionType::PlayerAttributeAbove:
			HandleTalentConditionAttribute(OutValue, SkillTalent, TagsToCaptureDefs, TagsCurrentAttributeToMaxAttribute, ExecutionParams,
							   EvaluationParameters, true);
			break;
		case ETalentConditionType::PlayerHasTag:
			if (SourceASC->HasMatchingGameplayTag(SkillTalent.TalentCondition.ConditionTag))
			{
				ChangeValueByTalentModifier(OutValue, SkillTalent);
			}
			break;
		case ETalentConditionType::PlayerDontHaveTag:
			if (!SourceASC->HasMatchingGameplayTag(SkillTalent.TalentCondition.ConditionTag))
			{
				ChangeValueByTalentModifier(OutValue, SkillTalent);
			}
			break;
		// Target.
		case ETalentConditionType::TargetAttributeBelow:
			HandleTalentConditionAttribute(OutValue, SkillTalent, TagsToCaptureDefs, TagsCurrentAttributeToMaxAttribute, ExecutionParams,
							   EvaluationParameters, false);
			break;
		case ETalentConditionType::TargetAttributeAbove:
			HandleTalentConditionAttribute(OutValue, SkillTalent, TagsToCaptureDefs, TagsCurrentAttributeToMaxAttribute, ExecutionParams,
							   EvaluationParameters, true);
			break;
		case ETalentConditionType::TargetHasTag:
			if (TargetASC->HasMatchingGameplayTag(SkillTalent.TalentCondition.ConditionTag))
			{
				ChangeValueByTalentModifier(OutValue, SkillTalent);
			}
			break;
		case ETalentConditionType::TargetDontHaveTag:
			if (!TargetASC->HasMatchingGameplayTag(SkillTalent.TalentCondition.ConditionTag))
			{
				ChangeValueByTalentModifier(OutValue, SkillTalent);
			}
			break;
		default:
			break;
		}
	}
}

void USkills_ExecCalc_Damage::ChangeValueByTalentModifier(float& OutValue, const FSkillTalent& SkillTalent) const
{
	if (SkillTalent.TalentType == ETalentType::AttributeAdditive)
	{
		OutValue += SkillTalent.TalentMagnitude * SkillTalent.TalentLevel;
	}
	if (SkillTalent.TalentType == ETalentType::AttributeMultiplicative)
	{
		OutValue *=  1 + ((SkillTalent.TalentMagnitude / 100) * SkillTalent.TalentLevel);
	}
}

void USkills_ExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                     FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, SkillDamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, SkillDamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, SkillDamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, SkillDamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, SkillDamageStatics().CriticalHitDamageDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, SkillDamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, SkillDamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, SkillDamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, SkillDamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, SkillDamageStatics().PhysicalResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Health, SkillDamageStatics().HealthDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_MaxHealth, SkillDamageStatics().MaxHealthDef);

	TMap<FGameplayTag, FGameplayTag> TagsCurrentAttributeToMaxAttribute;
	TagsCurrentAttributeToMaxAttribute.Add(Tags.Attributes_Secondary_Health, Tags.Attributes_Secondary_MaxHealth);
	// We could also do mana.
	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	// ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	// ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	const FGameplayTag& SkillTag = 	UAuraAbilitySystemLibrary::GetSkillTag(EffectContextHandle);
	TArray<FSkillTalent> SkillTalents = TArray<FSkillTalent>();
	if (SourceAvatar->Implements<UPlayerInterface>() && SkillTag.IsValid())
	{
		SkillTalents = IPlayerInterface::Execute_GetTalentsForSkill(SourceAvatar, SkillTag);
	}
	
	// Debuff
	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs, SkillTalents);
	
	// Get Damage Set by Caller Magnitude
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : Tags.DamageTypesToResistances)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& ResistanceTag = Pair.Value;
		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDef doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];

		// Get Damage Set by Caller Magnitude. We add all the different damages types (fire, lightning, etc).
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
		// We did not cause damage for this damage type. Its unnecessary to go further.
		if (FMath::IsNearlyZero(DamageTypeValue)) continue;
		
		// Target Resistance to the DamageType
		float TargetResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, TargetResistance);
		TargetResistance = FMath::Max<float>(0.f, TargetResistance);
		DamageTypeValue *=  (100.f - TargetResistance) / 100.f;

		if (!SkillTalents.IsEmpty())
		{
			HandleTalentsModifications(DamageTypeValue, DamageTypeTag, SkillTalents, TagsToCaptureDefs, TagsCurrentAttributeToMaxAttribute, ExecutionParams, EvaluationParameters, SourceASC, TargetASC);
		}
		
		// Radial Damage handling.
		if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			// 1. Override takeDamage in AuraCharacterBase. *
			// 2. Create delegate OnDamageDelegate, broadcast damage received in TakeDamage *
			// 3. Bind lambda to OnDamageDelegate on the Victim here. *
			// 4. In Lambda, SetDamageTypeValue to the damage received from the broadcast
			// 5. Call UGameplayStatics::ApplyRadialDamageWithFallof to cause damage (this will result in TakeDamage being called
			//		on the Victim, which will then broadcast OnDamageDelegate)
			if (ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar))
			{
				TargetCombatInterface->GetOnDamageSignature().AddLambda([&](float DamageAmount)
				{
					DamageTypeValue = DamageAmount;
				});
			}

			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvatar,
				DamageTypeValue,
				0.f,
				UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatar,
				nullptr);
		}
		
		Damage += DamageTypeValue;
	}

	// Capture BlockChance on Target, and determine if there was a successful Block.
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SkillDamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	if (bBlocked)
	{
		// If Block, halve the damage.
		Damage *= 0.5f;
	}
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	float TargetArmor = 0.f;
	// We will be able to use the attributes captured in the constructor like RelevantAttributesToCapture.Add(SkillDamageStatics().ArmorDef);
	// Very similar to MMC_MaxHealth GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SkillDamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	// We will now have the magnitude value for Armor.
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SkillDamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
	
	// ArmorPenetration (from Source) ignores a percentage of the Target's Armor.
	const float EffectiveArmor = TargetArmor *= ( 100 - SourceArmorPenetration * ArmorPenetrationCoefficient ) / 100.f;

	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	// Armor ignores a percentage of incoming damage.
	Damage *= ( 100 - EffectiveArmor * EffectiveArmorCoefficient ) / 100.f;

	// Check if Critical Hit.
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SkillDamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	// TODO TalentCondition ?
	const float SkillCriticalChance = Spec.GetSetByCallerMagnitude(Tags.Skills_Attributes_CriticalHitChance, false, 0);
	SourceCriticalHitChance += SkillCriticalChance;
	SourceCriticalHitChance = FMath::Max<float>(0.f, SourceCriticalHitChance);


	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SkillDamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	
	// If it is a Critical Hit.
	const bool bCritical = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
	if (bCritical)
	{
		float SourceCriticalDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SkillDamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalDamage);
		// TODO TalentCondition ?
		const float SkillCriticalDamage = Spec.GetSetByCallerMagnitude(Tags.Skills_Attributes_CriticalHitDamage, false, 0);
		SourceCriticalDamage += SkillCriticalDamage;
		SourceCriticalDamage = FMath::Max<float>(0.f, SourceCriticalDamage);
		Damage = 2 * Damage + SourceCriticalDamage;
	}
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCritical);

	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
