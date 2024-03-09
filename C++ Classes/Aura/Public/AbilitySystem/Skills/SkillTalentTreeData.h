// Copyright Nono Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SkillTalentTreeData.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ETalentType
{
	AttributeMultiplicative,
	AttributeAdditive,
	GameplayEffect
};

UENUM(BlueprintType)
enum class ETalentConditionType
{
	None,
	TargetAttributeBelow,
	TargetAttributeAbove,
	TargetHasTag,
	TargetDontHaveTag,
	TargetReceiveTag,
	PlayerAttributeBelow,
	PlayerAttributeAbove,
	PlayerHasTag,
	PlayerDontHaveTag,
};

USTRUCT(BlueprintType)
struct FTalentCondition
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETalentConditionType ConditionType = ETalentConditionType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ConditionAttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttributeValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool AttributeValueInPercent = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ConditionTag = FGameplayTag();
};

USTRUCT(BlueprintType)
struct FSkillTalent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SkillTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag TalentTag = FGameplayTag();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TalentLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 TalentMaxLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETalentType TalentType = ETalentType::AttributeAdditive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Tooltip="Always in percent if AttributeMultiplicative."))
	float TalentMagnitude = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTalentCondition TalentCondition = FTalentCondition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> TalentEffectClass;
};

USTRUCT(BlueprintType)
struct FTalentData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag TalentTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText TalentDisplayName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText TalentDescription = FText();

	// This const mean we can't change it at RunTime. It dont mean we can't set it in the blueprint.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> AbilityIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSkillTalent SkillTalent = FSkillTalent();
};

UCLASS()
class AURA_API USkillTalentTreeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TalentsInformations")
	TArray<FTalentData> TalentsInformations;

	const FTalentData* GetTalentData(const FGameplayTag& TalentTag) const;
};
