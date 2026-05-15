// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	void MoveToMouseCursor();
	void DashInput();
	void BasicAttackInput();
	
	bool GetMouseWorldLocation(FVector& OutWorldLocation) const;
};
