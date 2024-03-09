// Copyright Nono Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/Skills/SkillTalentTreeData.h"
#include "Skills_ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USkills_ExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	
public:
	USkills_ExecCalc_Damage();
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
						 const FGameplayEffectSpec& Spec,
						 FAggregatorEvaluateParameters EvaluationParameters,
						 const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDef,
						 TArray<FSkillTalent> SkillTalents) const;

	
	void HandleTalentsModifications(float& OutValue,
		const FGameplayTag& ValueAttribute,
		const TArray<FSkillTalent> SkillTalents,
		TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs,
		TMap<FGameplayTag, FGameplayTag> TagsCurrentAttributeToMaxAttribute,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FAggregatorEvaluateParameters EvaluationParameters,
		const UAbilitySystemComponent* SourceASC,
		const UAbilitySystemComponent* TargetASC
		) const;

	void HandleTalentConditionAttribute(float& OutValue,
		FSkillTalent SkillTalent,
		TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs,
		TMap<FGameplayTag, FGameplayTag> TagsCurrentAttributeToMaxAttribute,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FAggregatorEvaluateParameters EvaluationParameters,
		bool ConditionAttributeAboveThresold = false) const;
	

	void ChangeValueByTalentModifier(float& OutValue, const FSkillTalent& SkillTalent) const;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
