// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPlayerCharacterBase.h"
#include "LPlayerSkillSlot.h"
#include "LPlayerCharacter.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTLAR_API ALPlayerCharacter : public ALPlayerCharacterBase
{
	GENERATED_BODY()
	
public:
	ALPlayerCharacter();
	
	virtual void Dash(const FVector& DashDirection);
	
	void BasicAttack(const FVector& TargetLocation);
	
	virtual void CancelCurrentAction() override;
	
	void UseSkill(ELPlayerSkillSlot SkillSlot, const FVector& TargetLocation);
	
protected:
	void EndBasicAttack();
	
	void UseQSkill(const FVector& TargetLocation);
	void UseWSkill(const FVector& TargetLocation);
	void UseESkill(const FVector& TargetLocation);
	void UseRSkill(const FVector& TargetLocation);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	TObjectPtr<UNiagaraSystem> BasicAttackNiagara;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackDuration = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackForwardOffset = 80.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackHeightOffset = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	float BlinkDuration = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	TObjectPtr<UNiagaraSystem> BlinkStartEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	TObjectPtr<UNiagaraSystem> BlinkEndEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Q")
	TObjectPtr<UNiagaraSystem> QMeteorNiagara;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Q")
	float QMeteorDuration = 0.7f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Q")
	float QMeteorSpawnHeight = 1000.f;
	
private:
	FTimerHandle BasicAttackTimerHandle;
	
	FTimerHandle BlinkTimerHandle;
	
	FTimerHandle SkillTimerHandle;
	
	void EndSkill();
	void EndBlink();
	
	
	
};