// Copyright Nono Studios


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/BlessingData.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));
	
	NetUpdateFrequency = 100.f;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
	DOREPLIFETIME(AAuraPlayerState, SkillsTalents);
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXpChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXpChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToAttributePoints(int32 InAttributePoints)
{
	AttributePoints += InAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InSpellPoints)
{
	SpellPoints += InSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	OnXpChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(OldAttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(OldSpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::AddNewTalent(const FGameplayTag& TalentTag, const FGameplayTag& SkillTag, const FTalentData& TalentData)
{
	FSkillTalent* Talent = GetTalent(TalentTag);
	if (!Talent)
	{
		 FSkillTalent NewTalent = TalentData.SkillTalent;
		 SkillsTalents.Add(NewTalent);

		if (IsValid(NewTalent.TalentEffectClass))
		{
			UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
			AuraASC->ServerHandleTalent(TalentTag, SkillTag, NewTalent.TalentEffectClass, false);
		}
		
		AddToSpellPoints(-1);
	}
}

bool AAuraPlayerState::ChangeTalentLevel(const FGameplayTag& TalentTag, int32 InLevel)
{
	FSkillTalent* Talent = GetTalent(TalentTag);
	if (!Talent)
	{
		return false;
	}
	
	if (InLevel > 0 && Talent->TalentLevel >= Talent->TalentMaxLevel)
	{
		return false;
	}

	// Normally impossible, but another check don't hurt.
	if (InLevel < 0 && Talent->TalentLevel <= 0)
	{
		return false;
	}

	int32 PreviousLevel = Talent->TalentLevel;
	Talent->TalentLevel += InLevel;
	bool DeactivateTalent = false;
	if (PreviousLevel > 0 &&  Talent->TalentLevel <= 0)
	{
		// We have despecialized the talent.
		DeactivateTalent = true;
	}

	if (IsValid(Talent->TalentEffectClass))
	{
		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		AuraASC->ServerHandleTalent(Talent->SkillTag, TalentTag , Talent->TalentEffectClass, DeactivateTalent);
	}

	if (DeactivateTalent)
	{
		RemoveTalent(TalentTag);
	}
	
	AddToSpellPoints(-InLevel);
	return true;
}

bool AAuraPlayerState::HasTalent(const FGameplayTag& TalentTag)
{
	for (FSkillTalent& SkillTalent : SkillsTalents)
	{
		if (SkillTalent.TalentTag == TalentTag)
		{
			return true;
		}
	}
	return false;
}

int32 AAuraPlayerState::GetTalentLevel(const FGameplayTag& TalentTag)
{
	for (FSkillTalent& SkillTalent : SkillsTalents)
	{
	//	UE_LOG(LogTemp, Display, TEXT("SkillTalentTag : %s"), *TalentTag.ToString());
		if (SkillTalent.TalentTag == TalentTag)
		{
			return SkillTalent.TalentLevel;
		}
	}
	return 0;
}

FSkillTalent* AAuraPlayerState::GetTalent(const FGameplayTag& TalentTag)
{
	for (FSkillTalent& SkillTalent : SkillsTalents)
	{
		if (SkillTalent.TalentTag == TalentTag)
		{
			return &SkillTalent;
		}
	}
	return nullptr;
}

void AAuraPlayerState::RemoveTalent(const FGameplayTag& TalentTag)
{
	int32 i =0;
	TArray<FSkillTalent> CopyTalents = SkillsTalents;
	for (FSkillTalent& SkillTalent : CopyTalents)
	{
		if (SkillTalent.TalentTag == TalentTag)
		{
			SkillsTalents.RemoveAt(i);
		}
		i++;
	}
}

TArray<FSkillTalent> AAuraPlayerState::GetTalentsForSkill(const FGameplayTag& SkillTag)
{
	TArray<FSkillTalent> List = TArray<FSkillTalent>();
	for (FSkillTalent& SkillTalent : SkillsTalents)
	{
		if (SkillTalent.SkillTag == SkillTag)
		{
			List.Add(SkillTalent);
		}
	}
	return List;
}


void AAuraPlayerState::OnRep_Talents(TArray<FSkillTalent> OldTalents)
{
	// TODO Broadcast delegate
}
