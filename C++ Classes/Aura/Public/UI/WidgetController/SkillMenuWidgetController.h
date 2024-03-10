#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Skills/SkillTalentTreeData.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SkillMenuWidgetController.generated.h"

class USplineWidget;
class UWidget;
class UAuraUserWidget;
struct FTalentData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillGlobeSelectedSignature, const FGameplayTag&, AbilityTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTalentSignature, const FGameplayTag&, AbilityTag, const FGameplayTag&, TalentTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRespecActivateSignature, bool, Respec);

USTRUCT(BlueprintType)
struct FTalentHierarchy
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAuraUserWidget* ParentTalentNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidget* ParentSplineConnector;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USkillMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcasInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Skills")
	FOnPlayerStateChangedSignature SpellPointsDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Skills")
	FSkillGlobeSelectedSignature SkillGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Skills")
	FTalentSignature TalentDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Skills")
	FRespecActivateSignature RespecDelegate;
	
	UFUNCTION(BlueprintCallable)
	void SkillGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void EquippedSkillGlobeButtonPressed(const FGameplayTag& AbilityTag);
	
	UFUNCTION(BlueprintCallable)
	TArray<FTalentData> GetListTalentsData(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	const FTalentData GetDataForTalent(const FGameplayTag& AbilityTag, const FGameplayTag& TalentTag);
	
	UFUNCTION(BlueprintCallable)
	void SpendTalentsPoints(const FGameplayTag& TalentTag);

	UFUNCTION(BlueprintCallable)
	FString GetTextTalentLevel(const FTalentData& TalentData);

	UFUNCTION(BlueprintCallable)
	FString GetTextTalentDescription(const FTalentData& TalentData);

	UFUNCTION(BlueprintCallable)
	bool ActivateTalentNodeWidget(const FGameplayTag& AbilityTag, const FGameplayTag& TalentTag);

	UFUNCTION(BlueprintCallable)
	bool RespecButtonPressed();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TMap<FGameplayTag, TObjectPtr<USkillTalentTreeData>> AbilitiesTreesData;
	
private:
	FGameplayTag SelectedSkill;

	bool bRespecActivated = false;
};
