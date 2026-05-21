// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPlayerSkillSlot.h"
#include "GameFramework/PlayerController.h"
#include "LPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class PROJECTLAR_API ALPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
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
	TObjectPtr<UInputAction> SkillRAction;
	

	void MoveToMouseCursor();
	void DashInput();
	void BasicAttackInput();
	
	bool GetMouseWorldLocation(FVector& OutWorldLocation) const;
	
	void SkillQInput();
	void SkillWInput();
	void SkillEInput();
	void SkillRInput();
	
	void HandleSkillInput(ELPlayerSkillSlot SkillSlot);
};
