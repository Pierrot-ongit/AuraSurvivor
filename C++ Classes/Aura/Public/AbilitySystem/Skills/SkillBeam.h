// Copyright Nono Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Skills/SkillDamageGameplayAbility.h"
#include "SkillBeam.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USkillBeam : public USkillDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);
	
	UFUNCTION(BlueprintCallable)
	AActor* FindNextTarget(const FVector& PreviousTargetLocation);

	// The targets/enemies that have already been hit by the chain spell.
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TArray<AActor*> TargetsChained;

	UFUNCTION(BlueprintCallable)
	int32 GetMaxNumChainTargets();
	
protected:

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	float BeamTraceRadius = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	float BeamChainRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Beam")
	int32 MaxNumChainTargets = 3;
};
