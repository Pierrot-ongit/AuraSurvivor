// Copyright Nono Studios


#include "UI/WidgetController/SkillMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USkillMenuWidgetController::BroadcasInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USkillMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
[this](const int32 NewSpellPoints)
	{
		SpellPointsDelegate.Broadcast(NewSpellPoints);
	}
);
}

void USkillMenuWidgetController::SkillGlobeSelected(const FGameplayTag& AbilityTag)
{
	SkillGlobeSelectedDelegate.Broadcast(AbilityTag);
	SelectedSkill = AbilityTag;
}

void USkillMenuWidgetController::EquippedSkillGlobeButtonPressed(const FGameplayTag& AbilityTag)
{
	if (!SelectedSkill.MatchesTagExact(AbilityTag))
	{
		SkillGlobeSelected(AbilityTag);
	}
}

TArray<FTalentData> USkillMenuWidgetController::GetListTalentsData(const FGameplayTag& AbilityTag)
{
	const USkillTalentTreeData* AbilityTreeData = AbilitiesTreesData.FindChecked(AbilityTag);
	if (AbilityTreeData == nullptr) return TArray<FTalentData>();
	return AbilityTreeData->TalentsInformations;
}

// Most likely to use after TalentDelegate in TalentNode Widget.
const FTalentData USkillMenuWidgetController::GetDataForTalent(const FGameplayTag& AbilityTag, const FGameplayTag& TalentTag)
{
	const USkillTalentTreeData* AbilityTreeData = AbilitiesTreesData.FindChecked(AbilityTag);
	if (AbilityTreeData == nullptr) return FTalentData();

	const FTalentData* Data = AbilityTreeData->GetTalentData(TalentTag);
	if (Data)
	{
		return *Data;
	}
	return FTalentData();
}

void USkillMenuWidgetController::SpendTalentsPoints(const FGameplayTag& TalentTag)
{
	//UE_LOG(LogTemp, Display, TEXT("TalentTag : [%s]"), *TalentTag.ToString());
	const FTalentData Data = GetDataForTalent(SelectedSkill, TalentTag);
	if (!Data.TalentTag.IsValid()) return;
	
	int32 TalentLevel = GetAuraPS()->GetTalentLevel(TalentTag);
	
	int32 SkillPoint = 1;
	if (!bRespecActivated)
	{
		// If we are not doing respec, we should not spend points if we are not able to.
		if (GetAuraPS()->GetSpellPoints() < 1) return;
		if (TalentLevel >= Data.SkillTalent.TalentMaxLevel) return;
	}
	else
	{
		// We are going to refund the point instead of spending it.
		SkillPoint = -SkillPoint;
	}

	if (GetAuraPS()->HasTalent(TalentTag))
	{
		GetAuraPS()->ChangeTalentLevel(TalentTag, SkillPoint);
	}
	else
	{
		GetAuraPS()->AddNewTalent(TalentTag, SelectedSkill, Data);
	}
	
	TalentDelegate.Broadcast(SelectedSkill, TalentTag);
}

FString USkillMenuWidgetController::GetTextTalentLevel(const FTalentData& TalentData)
{
	int32 TalentLevel = GetAuraPS()->GetTalentLevel(TalentData.TalentTag);
	return FString::Printf(TEXT("%d / %d"),
	// Values
	TalentLevel,
	TalentData.SkillTalent.TalentMaxLevel);
}

FString USkillMenuWidgetController::GetTextTalentDescription(const FTalentData& TalentData)
{
	// <Damage>%d</> <Damage>{0}</>
	int32 Level = GetAuraPS()->GetTalentLevel(TalentData.TalentTag);
	if (Level < 1) Level = 1;
	int32 Magnitude = FMath::RoundToInt(Level * TalentData.SkillTalent.TalentMagnitude);
	FString Description = TalentData.TalentDescription.ToString();
	return FString::Format(*Description, {Magnitude});
}

bool USkillMenuWidgetController::ActivateTalentNodeWidget(const FGameplayTag& AbilityTag, const FGameplayTag& TalentTag)
{
	const FTalentData Data = GetDataForTalent(AbilityTag, TalentTag);
	if (!Data.TalentTag.IsValid()) return false;
	int32 Level = GetAuraPS()->GetTalentLevel(TalentTag);
	
	if (bRespecActivated)
	{
		if (Level > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	if (GetAuraPS()->GetSpellPoints() < 1) return false;
	if (Level >= Data.SkillTalent.TalentMaxLevel) return false;
	
	return true;
}

bool USkillMenuWidgetController::RespecButtonPressed()
{
	bRespecActivated = !bRespecActivated;
	RespecDelegate.Broadcast(bRespecActivated);
	return  bRespecActivated;
}
