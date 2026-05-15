// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LPlayerActionState.h"
#include "LPlayerCharacterBase.generated.h"

UCLASS()
class PROJECTLAR_API ALPlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ALPlayerCharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USpringArmComponent> CameraBoomComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCameraComponent> CameraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> PlayerHPWidget;
	
	virtual void Dash(const FVector& DashDirection);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashPower = 10000.f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	bool bBlink = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ELPlayerActionState CurrentActionState = ELPlayerActionState::Idle;

public:
	ELPlayerActionState GetCurrentActionState() const;
	
	void SetCurrentActionState(ELPlayerActionState NewState);
	
	bool CanMove() const;
	bool CanBasicAttack() const;
	bool CanDash() const;
	
	virtual void CancelCurrentAction();
};