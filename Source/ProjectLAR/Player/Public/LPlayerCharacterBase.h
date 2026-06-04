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
	
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashPower = 10000.f;*/

public:
	void CameraZoomIn();
	void CameraZoomOut();


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	bool bBlink = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ELPlayerActionState CurrentActionState = ELPlayerActionState::Idle;
	
	void UpdateDash(float DeltaTime);
	void EndDash();
	void ResetDashCooldown();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	float DashDistance = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	float DashDuration = 0.15f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	float DashCooldown = 2.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	bool bIsDashing = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	bool bCanDash = true;
	
	// =========================
	// Camera Zoom
	// =========================

	void UpdateCameraZoom(float DeltaTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float MinCameraArmLength = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float MaxCameraArmLength = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float CameraZoomStep = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float CameraZoomInterpSpeed = 8.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Zoom")
	float TargetCameraArmLength = 1500.0f;
	
private:
	FVector DashStartLocation;
	FVector DashTargetLocation;
	float DashElapsedTime = 0.f;
	
	FTimerHandle DashCooldownTimerHandle;
	
public:
	ELPlayerActionState GetCurrentActionState() const;
	
	void SetCurrentActionState(ELPlayerActionState NewState);
	
	bool CanMove() const;
	bool CanBasicAttack() const;
	bool CanDash() const;
	bool CanUseSkill() const;
	
	virtual void CancelCurrentAction();
	
public:
	bool IsDashing() const;
	bool IsDashOnCooldown() const;
	float GetDashCooldownRemaining() const;
	float GetDashCooldownRatio() const;
};