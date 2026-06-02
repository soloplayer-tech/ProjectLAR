// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LFloatingDamageActor.h"
#include "OB_CombatComponent.h"
#include "OB_LogManager.h"
#include "OB_PatternComponent.h"
#include "GameFramework/Character.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "OB_BossCharacter.generated.h"

class UOB_BossFSMComponent;

/**
 * @brief '컴포넌트들을 누가 들고 있지?' → ABossCharacter
 */

UCLASS()
class PROJECTLAR_API AOB_BossCharacter : public ACharacter, public ILDamageable
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	UOB_BossFSMComponent* FSMComponent;
	
	UPROPERTY(VisibleAnywhere)
	UOB_PatternComponent* PatternComponent;
	bool bDestroyOnDeath = true;
	float CurHP;
	float MaxHP = 1000.f;

public:
	// Sets default values for this character's properties
	AOB_BossCharacter();
	
	UOB_BossFSMComponent* GetFSMComponent() const { LOG_TRACE_INFO("Call GetFSMComponent"); return FSMComponent; }
	UOB_PatternComponent* GetPatternComponent() const { LOG_TRACE_INFO("Call GetPatternComponent"); return PatternComponent; };

	virtual void ReceiveSkillDamage_Implementation(
		float Damage,
		AActor* DamageCauser,
		ELPlayerSkillID SkillID
	) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ALFloatingDamageActor> FloatingDamageActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	float FloatingDamageHeightOffset = 140.0f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
private:
	void Die();
};
