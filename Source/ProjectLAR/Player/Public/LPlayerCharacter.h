// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPlayerCharacterBase.h"
#include "LPlayerSkillSlot.h"
#include "LPlayerSkillID.h"

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
	
	virtual void Tick(float DeltaSeconds) override;
	
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
	
	void EquipSkillToSlot(
	ELPlayerSkillSlot SkillSlot,
	ELPlayerSkillID SkillID
);
	
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
	// Player Resource Getter
	
	UFUNCTION(BlueprintPure, category = "Player|Resource")
	float GetHPRatio() const;

	UFUNCTION(BlueprintPure, category = "Player|Resource")
	float GetManaRatio() const;
	
	UFUNCTION(BlueprintPure, category = "Player|Resource")
	float GetIdentityRatio() const;
	
	UFUNCTION(BlueprintPure, category = "Player|Resource")
	float GetCurrentHP() const;
	
	UFUNCTION(BlueprintPure, category = "Player|Resource")
	float GetCurrentMana() const;
	
	UFUNCTION(BlueprintPure, category = "Player|Resource")
	float GetCurrentIdentityGauge() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Identity")
	void BP_OnIdentityActivated();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Identity")
	void BP_OnIdentityEnded();
	
	bool CanSpendMana(float ManaCost) const;
	bool SpendMana(float ManaCost);
	
	void RecoverMana(float Amount);
	void AddIdentityGauge(float Amount);
	bool CanActivateIdentity() const;
	void ActivateIdentity();
	void EndIdentity();

	bool IsIdentityFull() const;
	
	void OnSkillHitConfirmed(ELPlayerSkillID SkillID, int32 HitCount);
	bool IsIdentityActive() const;

	float GetFinalSkillDamage(float BaseDamage, ELPlayerSkillID SkillID) const;
	
	
	
protected:
	virtual void BeginPlay() override;
	
	void EndBasicAttack();
	void EndSkill();
	void EndBlink();
	
	void StartIdentityBuffVFX();
	void StopIdentityBuffVFX();
	
	
	float GetIdentityGainBySkill(ELPlayerSkillID SkillID) const;
	float GetIdentityAdditionalGainBySkill(ELPlayerSkillID SkillID) const;
	float GetIdentityMaxGainBySkill(ELPlayerSkillID SkillID) const;
	float GetIdentityGainByHitCount(ELPlayerSkillID SkillID, int32 HitCount) const;
	void ClearAllSkillCooldowns();
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
	// Player HP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Resource|HP")
	float MaxHP = 10000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Resource|HP")
	float CurrentHP = 10000.f;
	
	// =======================================================================================
	// Player Mana
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Resource|Mana")
	float MaxMana = 10000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Resource|Mana")
	float CurrentMana = 10000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Resource|Mana")
	float ManaRegenPerSecond = 500.f;
	
	// =======================================================================================
	// Player Resource - Identity
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Resource|Identity")
	float MaxIdentityGauge = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Resource|Identity")
	float CurrentIdentityGauge = 0.f;
	
	// =======================================================================================
	// Identity Gain

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float MeteorIdentityGain = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float IceLanceIdentityGain = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float ThunderIdentityGain = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float WindIdentityGain = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float BasicAttackIdentityGain = 2.0f;

	// =======================================================================================
	// Identity Gain - Additional Per Extra Target

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float MeteorAdditionalIdentityGainPerTarget = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float IceLanceAdditionalIdentityGainPerTarget = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float ThunderAdditionalIdentityGainPerTarget = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float WindAdditionalIdentityGainPerTarget = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float BasicAttackAdditionalIdentityGainPerTarget = 0.0f;

	// =======================================================================================
	// Identity Gain - Max Per Hit Unit

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float MeteorMaxIdentityGainPerCast = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float IceLanceMaxIdentityGainPerImpact = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float ThunderMaxIdentityGainPerStrike = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float WindMaxIdentityGainPerCast = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float BasicAttackMaxIdentityGainPerAttack = 2.0f;

	// =======================================================================================
	// Identity Active

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Identity")
	bool bIdentityActive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float IdentityDuration = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float IdentityDamageMultiplier = 1.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float IdentityCastDurationMultiplier = 0.7f;

	FTimerHandle IdentityTimerHandle;
	
	// =======================================================================================
	// Identity VFX

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity|VFX")
	TObjectPtr<UNiagaraSystem> IdentityBuffVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity|VFX")
	FVector IdentityBuffVFXLocationOffset = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity|VFX")
	FRotator IdentityBuffVFXRotationOffset = FRotator::ZeroRotator;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveIdentityBuffVFXComponent;
	
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
	float GetSkillManaCost(ELPlayerSkillID SkillID) const;
	void RegenerateMana(float DeltaTime);
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
	
	bool ApplySkillDamageToActor(
		AActor* TargetActor,
		float Damage,
		ELPlayerSkillID SkillID
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
	float BasicAttackForwardOffset = 180.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	float BasicAttackHeightOffset = 0.0f;

	// =======================================================================================
	// Basic Attack Damage

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack|Damage")
	float BasicAttackDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack|Damage")
	float BasicAttackDamageCenterOffset = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack|Damage")
	float BasicAttackDamageHeightOffset = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack|Damage")
	FVector BasicAttackDamageBoxHalfExtent = FVector(280.0f, 80.0f, 80.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack|Debug")
	bool bDrawBasicAttackDamageDebug = true;
	
	void ApplyBasicAttackDamage(const FVector& AttackDirection);
	
	
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
	
	void ApplyWindDamage(const FVector& AttackDirection);
	
	// =======================================================================================
	// Wind Damage

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind|Damage")
	float WindDamage = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind|Damage")
	float WindDamageCenterOffset = 260.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind|Damage")
	float WindDamageHeightOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind|Damage")
	FVector WindDamageBoxHalfExtent = FVector(260.0f, 120.0f, 80.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Wind|Debug")
	bool bDrawWindDamageDebug = true;

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
	// Skill Mana Cost
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Mana")
	float MeteorManaCost = 1500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Mana")
	float IceLanceManaCost = 1200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Mana")
	float ThunderManaCost = 1500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Mana")
	float WindManaCost = 500.f;
	
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
	ELPlayerSkillID VSlotSkill = ELPlayerSkillID::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID ASlotSkill = ELPlayerSkillID::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID SSlotSkill = ELPlayerSkillID::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID DSlotSkill = ELPlayerSkillID::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill|Equip")
	ELPlayerSkillID FSlotSkill = ELPlayerSkillID::None;
	
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