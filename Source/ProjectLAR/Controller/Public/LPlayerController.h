// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "LPlayerSkillSlot.h"
#include "GameFramework/PlayerController.h"
#include "LPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ULPlayerUIWidget;
class ULSkillWindowWidget;

UCLASS()
class PROJECTLAR_API ALPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> IdentityAction;

	void IdentityInput();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> CameraZoomInAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> CameraZoomOutAction;
	
	void CameraZoomInInput();
	void CameraZoomOutInput();
	
	// =========================
	// Skill Window
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleSkillWindowAction;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<ULSkillWindowWidget> SkillWindowWidgetClass;

	UPROPERTY()
	TObjectPtr<ULSkillWindowWidget> SkillWindowWidget;

	void ToggleSkillWindowInput();
	void CreateSkillWindowWidget();
	void BindSkillWindowToPawn(APawn* InPawn);
	
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Player;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ClickMoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> PlayerDashAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> BasicAttackAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillQAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillWAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillEAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillAAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillSAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillDAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillFAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillRAction;
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	TObjectPtr<UInputAction> SkillVAction;
	
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<ULPlayerUIWidget> PlayerUIWidgetClass;
	
	UPROPERTY()
	TObjectPtr<ULPlayerUIWidget> PlayerUIWidget;
	
	void CreatePlayerUIWidget();
	

	void MoveToMouseCursor();
	void DashInput();
	void BasicAttackInput();
	
	bool GetMouseWorldLocation(FVector& OutWorldLocation) const;
	
	void SkillQInput();
	void SkillWInput();
	void SkillEInput();
	void SkillRInput();
	void SkillAInput();
	void SkillSInput();
	void SkillDInput();
	void SkillFInput();
	void SkillVInput();
	
	
	void HandleSkillInput(ELPlayerSkillSlot SkillSlot);
	
	void BindPlayerUIToPawn(APawn* InPawn);
};
