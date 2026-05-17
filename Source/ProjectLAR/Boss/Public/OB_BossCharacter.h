// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OB_BossFSMComponent.h"
#include "GameFramework/Character.h"
#include "OB_BossCharacter.generated.h"

UCLASS()
class PROJECTLAR_API AOB_BossCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	UOB_BossFSMComponent* FSMComponent;

public:
	// Sets default values for this character's properties
	AOB_BossCharacter();
	
	UFUNCTION()
	UOB_BossFSMComponent* GetFSMComponent() const { return FSMComponent; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
