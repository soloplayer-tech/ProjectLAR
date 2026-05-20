// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OB_TestController.h"
#include "GameFramework/Character.h"
#include "OB_TestCharacter.generated.h"

class UInputAction;

UCLASS()
class PROJECTLAR_API AOB_TestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOB_TestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, Category=TestVar)
	TObjectPtr<class USpringArmComponent> CameraBoomComp;
	
	UPROPERTY(EditAnywhere, Category=TestVar)
	TObjectPtr<class UCameraComponent> CameraComp;
	
	UPROPERTY(EditAnywhere, Category=TestVar)
	TObjectPtr<UInputAction> IA_Move;
	
	UPROPERTY(EditAnywhere, Category = TestVar)
	TObjectPtr<UInputAction> IA_Look;	
	
	UPROPERTY(EditAnywhere, Category = MyVar)
	TObjectPtr<class UInputMappingContext> IMC_TestPlayer;
	
	UPROPERTY(EditAnywhere, Category = TestVar)
	TObjectPtr<AOB_TestController> TestCtrl;
	
	void OnTestMove(const struct FInputActionValue& value);
	void OnTestLook(const struct FInputActionValue& value);
	
};
