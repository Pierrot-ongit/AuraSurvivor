// Copyright Nono Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Skills/SkillTalentTreeData.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

struct FSkillTalent;
struct FTalentData;
class UGameplayEffect;
class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32 /*StateValue*/)

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	FOnPlayerStateChanged OnXpChangedDelegate;
	FOnPlayerStateChanged OnLevelChangedDelegate;
	FOnPlayerStateChanged OnAttributePointsChangedDelegate;
	FOnPlayerStateChanged OnSpellPointsChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return  Level; }
	FORCEINLINE int32 GetXP() const { return  XP; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetAttributePoints() const { return  AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return  SpellPoints; }

	void AddToXP(int32 InXP);
	void AddToLevel(int32 InLevel);
	void AddToAttributePoints(int32 InAttributePoints);
	void AddToSpellPoints(int32 InSpellPoints);

	void SetXP(int32 InXP);
	void SetLevel(int32 InLevel);

	////// TALENTS
	FORCEINLINE TArray<FSkillTalent> GetTalents() const { return  SkillsTalents; }

	void AddNewTalent(const FGameplayTag& TalentTag, const FGameplayTag& SkillTag, const FTalentData& TalentData);
	
	bool ChangeTalentLevel(const FGameplayTag& TalentTag, int32 InLevel);

	bool HasTalent(const FGameplayTag& TalentTag);

	int32 GetTalentLevel(const FGameplayTag& TalentTag);
	
	FSkillTalent* GetTalent(const FGameplayTag& TalentTag);
	void RemoveTalent(const FGameplayTag& TalentTag);
	TArray<FSkillTalent> GetTalentsForSkill(const FGameplayTag& SkillTag);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 10;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);

	////// Talents ///////
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Talents)
	TArray<FSkillTalent> SkillsTalents;
	
	UFUNCTION()
	void OnRep_Talents(TArray<FSkillTalent> OldTalents);
};
