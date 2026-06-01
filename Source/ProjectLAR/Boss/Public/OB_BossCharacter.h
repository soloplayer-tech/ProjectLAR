// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OB_CombatComponent.h"
#include "OB_LogManager.h"
#include "OB_PatternComponent.h"
#include "GameFramework/Character.h"
#include "OB_BossCharacter.generated.h"

class UOB_BossFSMComponent;

/**
 * @brief '컴포넌트들을 누가 들고 있지?' → ABossCharacter
 */

UCLASS()
class PROJECTLAR_API AOB_BossCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	UOB_BossFSMComponent* FSMComponent;
	
	UPROPERTY(VisibleAnywhere)
	UOB_PatternComponent* PatternComponent;

public:
	// Sets default values for this character's properties
	AOB_BossCharacter();
	
	UOB_BossFSMComponent* GetFSMComponent() const { LOG_TRACE_INFO("Call GetFSMComponent"); return FSMComponent; }
	UOB_PatternComponent* GetPatternComponent() const { LOG_TRACE_INFO("Call GetPatternComponent"); return PatternComponent; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
