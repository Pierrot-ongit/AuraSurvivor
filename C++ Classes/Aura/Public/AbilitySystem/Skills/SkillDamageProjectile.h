#pragma once

#include "CoreMinimal.h"
#include "SkillDamageGameplayAbility.h"
#include "SkillDamageProjectile.generated.h"

class UGameplayEffect;
class AAuraProjectile;

UCLASS()
class USkillDamageProjectile : public USkillDamageGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch = false, float PitchOverride = 0.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	int32 NumberProjectiles = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	int32 MaxNumProjectiles = 5;
	
};
