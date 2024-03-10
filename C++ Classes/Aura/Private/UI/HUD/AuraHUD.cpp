// Copyright Nono Studios


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SkillMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);
		SpellMenuWidgetController->BindCallbacksToDependencies();
	}
	return SpellMenuWidgetController;
}

USkillMenuWidgetController* AAuraHUD::GetSkillMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (SkillMenuWidgetController == nullptr)
	{
		SkillMenuWidgetController = NewObject<USkillMenuWidgetController>(this, SkillMenuWidgetControllerClass);
		SkillMenuWidgetController->SetWidgetControllerParams(WCParams);
		SkillMenuWidgetController->BindCallbacksToDependencies();
	}
	return SkillMenuWidgetController;
}

UBlessingMenuWidgetController* AAuraHUD::GetBlessingMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (BlessingMenuWidgetController == nullptr)
	{
		BlessingMenuWidgetController = NewObject<UBlessingMenuWidgetController>(this, BlessingMenuWidgetControllerClass);
		BlessingMenuWidgetController->SetWidgetControllerParams(WCParams);
		BlessingMenuWidgetController->BindCallbacksToDependencies();
	}
	return BlessingMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{  
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	
	FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	OverlayWidget->SetWidgetController(WidgetController); // Only when this happen, that we can broadcast with initial values, with delegates.
	WidgetController->BroadcasInitialValues();
	
	Widget->AddToViewport();
}
