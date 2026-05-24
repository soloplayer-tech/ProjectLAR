// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPlayerCharacterBase.h"
#include "LPlayerSkillSlot.h"
#include "ProjectLAR/Skill/Public/LIceLanceActor.h"
#include "LMeteorActor.h"
#include "LThunderActor.h"
#include "LPlayerCharacter.generated.h"

class UNiagaraSystem;
class ALMeteorActor;

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
	
protected:
	void EndBasicAttack();
	
	void UseQSkill(const FVector& TargetLocation);
	void UseWSkill(const FVector& TargetLocation);
	void UseESkill(const FVector& TargetLocation);
	void UseRSkill(const FVector& TargetLocation);
	void UseVSkill(const FVector& TargetLocation);
	
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

	// =======================================================================================

	
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Q")
	TObjectPtr<UNiagaraSystem> QMeteorNiagara;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Q")
	float QMeteorDuration = 0.7f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Q")
	float QMeteorSpawnHeight = 1000.f;*/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q")
	TSubclassOf<ALMeteorActor> MeteorActorClass;

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

	// 준비 위치: 캐릭터 뒤쪽으로 얼마나 띄울지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyBackOffset = 70.0f;

	// 준비 위치: 좌우 간격
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadySideSpacing = 110.0f;

	// 준비 위치: 기본 높이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyHeight = 180.0f;

	// 가운데 창을 살짝 더 높게 만드는 값
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyHeightFalloff = 25.0f;

	// 베지어 곡선이 좌우로 휘는 정도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceCurveSideOffset = 180.0f;

	// 베지어 곡선이 위로 솟는 정도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceCurveHeightOffset = 120.0f;

	// 목표 지점에 꽂히는 높이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceEndHeightOffset = 60.0f;

	// 생성 후 발사까지의 준비 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceReadyDuration = 0.18f;

	// 발사 시간차
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float IceLanceFireInterval = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	float WSkillLockDuration = 0.35f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W")
	TObjectPtr<UNiagaraSystem> WIceLanceNiagara;
	
	// =======================================================================================
	
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E")
	TObjectPtr<UNiagaraSystem> EThunderNiagara;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|E")
	float EThunderDuration = 0.7f;*/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E")
	TSubclassOf<ALThunderActor> ThunderStormActorClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E")
	float ESkillLockDuration = 0.45f;
	
	// =======================================================================================

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	TObjectPtr<UNiagaraSystem> RWindNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	float RWindForwardOffset = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	float RWindHeightOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R")
	float RSkillLockDuration = 0.35f;
	
private:
	FTimerHandle BasicAttackTimerHandle;
	
	FTimerHandle BlinkTimerHandle;
	
	FTimerHandle SkillTimerHandle;
	
	void EndSkill();
	void EndBlink();
	
	
	
};