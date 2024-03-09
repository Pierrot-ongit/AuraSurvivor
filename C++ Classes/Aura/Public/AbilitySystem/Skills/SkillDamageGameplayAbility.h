// Copyright Nono Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystem/Skills/SkillTalentTreeData.h"
#include "SkillDamageGameplayAbility.generated.h"

struct FTalentData;
class USkillTalentTreeData;

/**
 * 
 */
UCLASS()
class AURA_API USkillDamageGameplayAbility : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	void SetupTalentTree();
	
	float GetTalentsModifiersForAttribute(float OutValue, const FGameplayTag& AttributeTag);

	virtual FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector InKnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector InDeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f
		) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkillTalentTreeData> SkillTalentTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, FSkillTalent> TalentsTree;

	const FTalentData* FindTalentDataForTag(const FGameplayTag& TalentTag, bool bLogNotFound = false) const;
	
	TArray<FSkillTalent> FindTalentsForAttribute(const FGameplayTag& AttributeTag) const;

	TArray<FTalentData> GetTalentsTree();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoCast = false;

protected:
	bool bTalentTreeSetup = false;

};
