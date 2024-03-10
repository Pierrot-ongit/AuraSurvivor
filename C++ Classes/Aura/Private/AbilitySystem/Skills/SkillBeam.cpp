// Copyright Nono Studios


#include "AbilitySystem/Skills/SkillBeam.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

FString USkillBeam::GetDescription(int32 Level)
{
	const int32 ScaledDamage =  Damage.GetValueAtLevel(Level);
	const int32 MaxTargets =  GetMaxNumChainTargets();
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		// Title
		//"<Title>FIRE BOLT</>\n\n"

		// Details
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Description
		"<Default>Launches a lightning chain, jumping to additional nearby enemies, up to a maximum of %d targets. </>\n"
		"<Default>Deal </><Damage>%d</><Default> lightning damage with a chance to stun</>"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		MaxTargets,
		ScaledDamage);
}

void USkillBeam::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void USkillBeam::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;
			// TipSocket could be a member variable so we can use this ability on enemy character.
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				BeamTraceRadius,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true);

			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
}

AActor* USkillBeam::FindNextTarget(const FVector& PreviousTargetLocation)
{
	check(OwnerCharacter);
	TArray<AActor*> ActorsToIgnore = TargetsChained;
	ActorsToIgnore.Add(OwnerCharacter);
	
	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(OwnerCharacter, OverlappingActors, ActorsToIgnore, BeamChainRadius, PreviousTargetLocation);
	
	AActor* ClosestTarget = UAuraAbilitySystemLibrary::GetClosestTarget(OverlappingActors, PreviousTargetLocation);
	// if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ClosestTarget))
	// {
	// 	// if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
	// 	// {
	// 	// 	CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
	// 	// }
	// 	// On a trouvé une nouvelle cible et elle est correcte.
	// 	//TargetsChained.Add(ClosestTarget);
	// }
	return ClosestTarget;
}

int32 USkillBeam::GetMaxNumChainTargets()
{
	// TODO Calculate with talents.
	return MaxNumChainTargets;
}
