// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LPlayerCharacterBase.generated.h"

UCLASS()
class PROJECTLAR_API ALPlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALPlayerCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class USpringArmComponent> CameraBoomComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCameraComponent> CameraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> PlayerHPWidget;

public:
	virtual void Dash(const FVector& DashDirection);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashPower = 3000.f;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Dash")
	bool bBlink = false;
	

};
