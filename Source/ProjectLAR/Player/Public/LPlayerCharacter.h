// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPlayerCharacterBase.h"
#include "LPlayerSkillSlot.h"
#include "TimerManager.h"

#include "ProjectLAR/Skill/Public/LIceLanceActor.h"
#include "LMeteorActor.h"
#include "LThunderActor.h"

#include "LPlayerCharacter.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTLAR_API ALPlayerCharacter : public ALPlayerCharacterBase
{
	GENERATED_BODY()
	
public:
	ALPlayerCharacter();
	
	virtual void Dash(const FVector& DashDirection) override;
	
	void BasicAttack(const FVector& TargetLocation);
	
	virtual void CancelCurrentAction() override;
	
	void UseSkill(ELPlayerSkillSlot SkillSlot, const FVector& TargetLocation);

	// Skill Cooldown
	bool CanUseSkillSlot(ELPlayerSkillSlot SkillSlot) const;

	bool IsSkillOnCooldown(ELPlayerSkillSlot SkillSlot) const;
	float GetSkillCooldownRemaining(ELPlayerSkillSlot SkillSlot) const;
	float GetSkillCooldownRatio(ELPlayerSkillSlot SkillSlot) const;
	
protected:
	void EndBasicAttack();
	void EndSkill();
	void EndBlink();

	// 스킬 함수는 성공 여부를 반환한다.
	bool UseQSkill(const FVector& TargetLocation);
	bool UseWSkill(const FVector& TargetLocation);
	bool UseESkill(const FVector& TargetLocation);
	bool UseRSkill(const FVector& TargetLocation);
	bool UseVSkill(const FVector& TargetLocation);

	// Cooldown
	void StartSkillCooldown(ELPlayerSkillSlot SkillSlot);
	float GetSkillCooldownDuration(ELPlayerSkillSlot SkillSlot) const;

	void ResetQSkillCooldown();
	void ResetWSkillCooldown();
	void ResetESkillCooldown();
	void ResetRSkillCooldown();
	void ResetVSkillCooldown();
	
protected:
	// =======================================================================================
	// Basic Attack
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	TObjectPtr<UNiagaraSystem> BasicAttackNiagara;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackDuration = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackForwardOffset = 80.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackHeightOffset = 0.0f;

	// =======================================================================================
	// Dash Blink
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	float BlinkDuration = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	TObjectPtr<UNiagaraSystem> BlinkStartEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	TObjectPtr<UNiagaraSystem> BlinkEndEffect;

	// =======================================================================================
	// Q Skill - Meteor
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q")
	TSubclassOf<ALMeteorActor> MeteorActorClass;

	// 행동 잠금 시간: Q 사용 후 캐릭터가 Skill 상태로 묶이는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q")
	float QSkillLockDuration = 0.5f;

	// =======================================================================================
	// W Skill - Bezier Ice Lance

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	TSubclassOf<ALIceLanceActor> IceLanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	int32 IceLanceCount = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceTravelDuration = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyBackOffset = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadySideSpacing = 110.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyHeight = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyHeightFalloff = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceCurveSideOffset = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceCurveHeightOffset = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceEndHeightOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyDuration = 0.18f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceFireInterval = 0.05f;

	// 행동 잠금 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float WSkillLockDuration = 0.35f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	TObjectPtr<UNiagaraSystem> WIceLanceNiagara;
	
	// =======================================================================================
	// E Skill - Thunder
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E")
	TSubclassOf<ALThunderActor> ThunderStormActorClass;
	
	// 행동 잠금 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E")
	float ESkillLockDuration = 0.45f;
	
	// =======================================================================================
	// R Skill - Wind

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	TObjectPtr<UNiagaraSystem> RWindNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	float RWindForwardOffset = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	float RWindHeightOffset = 60.0f;

	// 행동 잠금 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	float RSkillLockDuration = 0.35f;

	// =======================================================================================
	// Skill Cooldown - 재사용 대기시간

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float QSkillCooldown = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float WSkillCooldown = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float ESkillCooldown = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float RSkillCooldown = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float VSkillCooldown = 10.0f;
	
private:
	FTimerHandle BasicAttackTimerHandle;
	FTimerHandle BlinkTimerHandle;

	// 현재 스킬 행동 잠금용 공용 타이머
	FTimerHandle SkillTimerHandle;

	// 스킬별 재사용 쿨타임 타이머
	FTimerHandle QSkillCooldownTimerHandle;
	FTimerHandle WSkillCooldownTimerHandle;
	FTimerHandle ESkillCooldownTimerHandle;
	FTimerHandle RSkillCooldownTimerHandle;
	FTimerHandle VSkillCooldownTimerHandle;
};