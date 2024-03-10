// Copyright Nono Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/BlessingMenuWidgetController.h"
#include "AuraHUD.generated.h"

class UAttributeMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UAuraUserWidget;
class USpellMenuWidgetController;
class USkillMenuWidgetController;

/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	USpellMenuWidgetController* GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams);
	USkillMenuWidgetController* GetSkillMenuWidgetController(const FWidgetControllerParams& WCParams);
	UBlessingMenuWidgetController* GetBlessingMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:

	
private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USkillMenuWidgetController> SkillMenuWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USkillMenuWidgetController> SkillMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UBlessingMenuWidgetController> BlessingMenuWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlessingMenuWidgetController> BlessingMenuWidgetControllerClass;
};
