// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPlayerCharacterBase.h"
#include "LPlayerSkillSlot.h"
#include "LPlayerSkillID.h"
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

	// 입력 슬롯 기준 스킬 사용
	void UseSkill(ELPlayerSkillSlot SkillSlot, const FVector& TargetLocation);

	// =======================================================================================
	// Skill Equip
	// Q/W/E/R 같은 슬롯에 어떤 실제 스킬이 들어있는지 관리한다.

	UFUNCTION(BlueprintPure, Category = "Skill|Equip")
	ELPlayerSkillID GetEquippedSkillID(ELPlayerSkillSlot SkillSlot) const;
	
	UFUNCTION(BlueprintCallable, Category = "Skill|Equip")
	void SetEquippedSkillID(ELPlayerSkillSlot SkillSlot, ELPlayerSkillID SkillID);

	// =======================================================================================
	// Skill Cooldown
	// Controller는 Slot 기준으로 들어오지만,
	// 실제 쿨타임은 SkillID 기준으로 판단한다.

	bool CanUseSkillSlot(ELPlayerSkillSlot SkillSlot) const;
	bool CanUseSkillID(ELPlayerSkillID SkillID) const;

	bool IsSkillIDOnCooldown(ELPlayerSkillID SkillID) const;
	float GetSkillIDCooldownRemaining(ELPlayerSkillID SkillID) const;
	float GetSkillIDCooldownRatio(ELPlayerSkillID SkillID) const;
	
	// =======================================================================================
	


protected:
	void EndBasicAttack();
	void EndSkill();
	void EndBlink();

	// =======================================================================================
	// Skill Execute
	// 현재는 이름이 UseQSkill/UseWSkill이지만,
	// 실제 의미는 Meteor/IceLance/Thunder/Wind 실행이다.
	// 나중에 이름을 UseMeteorSkill() 식으로 바꾸면 더 좋다.

	bool UseQSkill(const FVector& TargetLocation);
	bool UseWSkill(const FVector& TargetLocation);
	bool UseESkill(const FVector& TargetLocation);
	bool UseRSkill(const FVector& TargetLocation);
	bool UseVSkill(const FVector& TargetLocation);

	bool ExecuteSkillByID(
		ELPlayerSkillID SkillID,
		const FVector& TargetLocation
	);

	// =======================================================================================
	// Cooldown Internal - SkillID 기준

	void StartSkillCooldown(ELPlayerSkillID SkillID);
	float GetSkillCooldownDuration(ELPlayerSkillID SkillID) const;

	void ResetMeteorCooldown();
	void ResetIceLanceCooldown();
	void ResetThunderCooldown();
	void ResetWindCooldown();
	void ResetMeteorRainCooldown();
	
	// =======================================================================================
	// Casting
public:
	bool IsCasting() const;
	float GetCastRemaining() const;
	float GetCastRatio() const;
	ELPlayerSkillID GetCastingSkillID() const;
	
protected:
	
	float GetSkillCastDuration(ELPlayerSkillID SkillID) const;
	bool DoesSkillNeedCasting(ELPlayerSkillID SkillID) const;
	
	void StartSkillCast(
		ELPlayerSkillSlot SkillSlot,
		ELPlayerSkillID SkillID,
		const FVector& TargetLocation
		);
	
	void SpawnCastStartEffect(
		ELPlayerSkillID SkillID,
		const FVector& TargetLocation
		);
	
	void FinishSkillCast();
	void CancelSkillCast();
	
	// =======================================================================================
	// Skill Cast Duration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cast")
	float MeteorCastDuration = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cast")
	float ThunderCastDuration = 2.f;
	
	// =======================================================================================
	// Skill Casting Niagara
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cast|Effect")
	TObjectPtr<UNiagaraSystem> MeteorCastStartEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cast|Effect")
	TObjectPtr<UNiagaraSystem> ThunderCastStartEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cast|Effect")
	float MeteorCastEffectHeightOffset = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cast|Effect")
	float ThunderCastEffectHeightOffset = 5.f;
	
	
	
	
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
	// Meteor Skill
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Meteor")
	TSubclassOf<ALMeteorActor> MeteorActorClass;

	// 행동 잠금 시간: Meteor 발동 후 캐릭터가 Skill 상태로 묶이는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Meteor")
	float MeteorSkillLockDuration = 0.5f;
	

	// =======================================================================================
	// Ice Lance Skill

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	TSubclassOf<ALIceLanceActor> IceLanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	int32 IceLanceCount = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceTravelDuration = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceReadyBackOffset = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceReadySideSpacing = 110.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceReadyHeight = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceReadyHeightFalloff = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceCurveSideOffset = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceCurveHeightOffset = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceEndHeightOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceReadyDuration = 0.18f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceFireInterval = 0.05f;

	// 행동 잠금 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	float IceLanceSkillLockDuration = 0.35f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	TObjectPtr<UNiagaraSystem> WIceLanceNiagara;
	
	// =======================================================================================
	// Thunder Skill
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Thunder")
	TSubclassOf<ALThunderActor> ThunderStormActorClass;
	
	// 행동 잠금 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Thunder")
	float ThunderSkillLockDuration = 0.45f;
	
	// =======================================================================================
	// Wind Skill

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind")
	TObjectPtr<UNiagaraSystem> RWindNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind")
	float WindForwardOffset = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind")
	float WindHeightOffset = 60.0f;

	// 행동 잠금 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind")
	float WindSkillLockDuration = 0.35f;

	// =======================================================================================
	// Skill Cooldown - 실제 스킬 기준 재사용 대기시간

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float MeteorCooldown = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float IceLanceCooldown = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float ThunderCooldown = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float WindCooldown = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float MeteorRainCooldown = 10.0f;
	
	// =======================================================================================
	// Skill Equip - 슬롯에 장착된 실제 스킬

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID QSlotSkill = ELPlayerSkillID::Meteor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID WSlotSkill = ELPlayerSkillID::IceLance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID ESlotSkill = ELPlayerSkillID::Thunder;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID RSlotSkill = ELPlayerSkillID::Wind;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID VSlotSkill = ELPlayerSkillID::MeteorRain;
	
private:
	FTimerHandle BasicAttackTimerHandle;
	FTimerHandle BlinkTimerHandle;

	// 현재 스킬 행동 잠금용 공용 타이머
	FTimerHandle SkillTimerHandle;

	// 실제 스킬 기준 재사용 쿨타임 타이머
	FTimerHandle MeteorCooldownTimerHandle;
	FTimerHandle IceLanceCooldownTimerHandle;
	FTimerHandle ThunderCooldownTimerHandle;
	FTimerHandle WindCooldownTimerHandle;
	FTimerHandle MeteorRainCooldownTimerHandle;
	
	// 캐스팅 타이머
	FTimerHandle CastTimerHandle;
	
	bool bIsCasting = false;
	
	ELPlayerSkillSlot CastingSkillSlot = ELPlayerSkillSlot::Q;
	ELPlayerSkillID CastingSkillID = ELPlayerSkillID::None;
	
	FVector CastingTargetLocation = FVector::ZeroVector;
	
	float CurrentCastDuration = 0.0f;
};