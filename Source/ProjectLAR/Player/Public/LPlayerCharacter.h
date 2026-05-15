// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./../Public/LPlayerCharacterBase.h"
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
	
protected:
	void EndBasicAttack();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	TObjectPtr<UNiagaraSystem> BasicAttackNiagara;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackDuration = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackForwardOffset = 80.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackHeightOffset = 0.f;
	
private:
	FTimerHandle BasicAttackTimerHandle;
	
	float BlinkDuration = 0.5f;
	FTimerHandle BlinkTimerHandle;
	
	void EndBlink();
	
	
};