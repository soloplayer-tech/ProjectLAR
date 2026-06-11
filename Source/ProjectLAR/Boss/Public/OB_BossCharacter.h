// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OB_LogManager.h"
#include "GameFramework/Character.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "OB_BossCharacter.generated.h"

class ALFloatingDamageActor;
class UOB_PatternComponent;
class UOB_CombatComponent;
class UOB_BossFSMComponent;

/**
 * @brief '컴포넌트들을 누가 들고 있지?' → ABossCharacter
 */

UCLASS()
class PROJECTLAR_API AOB_BossCharacter : public ACharacter, public ILDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOB_BossCharacter();
	
	UFUNCTION()
	UOB_BossFSMComponent* GetFSMComponent() const { LOG_TRACE_INFO("Call GetFSMComponent"); return FSMComponent; }
	
	UFUNCTION()
	UOB_PatternComponent* GetPatternComponent() const { LOG_TRACE_INFO("Call GetPatternComponent"); return PatternComponent; }
	
	UFUNCTION()
	UOB_CombatComponent* GetCombatComponent() const { LOG_TRACE_INFO("Call GetCombatComponent"); return CombatComponent; }
	
	// AnimMontage
	UPROPERTY(EditDefaultsOnly, Category = "Boss Anim")
	UAnimMontage* RushMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Boss Anim")
	UAnimMontage* HammerMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Boss Anim")
	UAnimMontage* SlamMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Boss Anim")
	UAnimMontage* CardMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ALFloatingDamageActor> FloatingDamageActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	float FloatingDamageHeightOffset = 140.0f;


private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOB_BossFSMComponent> FSMComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOB_PatternComponent> PatternComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOB_CombatComponent> CombatComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void ReceiveSkillDamage_Implementation(float Damage, AActor* DamageCauser, ELPlayerSkillID SkillID) override;
	
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

};
