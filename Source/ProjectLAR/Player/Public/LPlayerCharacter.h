// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPlayerCharacterBase.h"
#include "LPlayerSkillSlot.h"
#include "LPlayerSkillID.h"
#include "LPlayerSkillDatabase.h"

#include "ProjectLAR/Skill/Public/LIceLanceActor.h"
#include "LGroundAreaSkillActor.h"
#include "LMeteorActor.h"
#include "LThunderActor.h"

#include "LPlayerCharacter.generated.h"

class UNiagaraSystem;
class UAnimMontage;
class UTexture2D;

UCLASS()
class PROJECTLAR_API ALPlayerCharacter : public ALPlayerCharacterBase
{
	GENERATED_BODY()
	
public:
	ALPlayerCharacter();
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void Dash(const FVector& DashDirection) override;
	
	void BasicAttack(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Combat|BasicAttack")
	void TriggerBasicAttackHit();
	
	virtual void CancelCurrentAction() override;

	// 입력 슬롯 기준 스킬 사용
	void UseSkill(ELPlayerSkillSlot SkillSlot, const FVector& TargetLocation);

	// 세이브 관련
	void SaveEquippedSkillSlots();
	void LoadEquippedSkillSlots();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Save")
	FString PlayerSaveSlotName = TEXT("PlayerSaveSlot");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Save")
	int32 PlayerSaveUserIndex = 0;
	
	// =======================================================================================
	// Skill Equip
	// Q/W/E/R 같은 슬롯에 어떤 실제 스킬이 들어있는지 관리한다.
public:
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

	UFUNCTION(BlueprintPure, Category = "Skill|Data")
	FText GetSkillDisplayName(ELPlayerSkillID SkillID) const;

	UFUNCTION(BlueprintPure, Category = "Skill|Data")
	UTexture2D* GetSkillIconTexture(ELPlayerSkillID SkillID) const;
	
	
	
protected:
	virtual void BeginPlay() override;
	
	void EndBasicAttack();
	void EndSkill();
	void EndBlink();
	bool PrepareActionDirection(
		const FVector& TargetLocation,
		FVector& OutDirection,
		FRotator& OutRotation
	);
	
	void StartIdentityBuffVFX();
	void StopIdentityBuffVFX();
	
	
	float GetIdentityGainBySkill(ELPlayerSkillID SkillID) const;
	float GetIdentityAdditionalGainBySkill(ELPlayerSkillID SkillID) const;
	float GetIdentityMaxGainBySkill(ELPlayerSkillID SkillID) const;
	float GetIdentityGainByHitCount(ELPlayerSkillID SkillID, int32 HitCount) const;
	void ClearAllSkillCooldowns();
	// =======================================================================================
	// Skill Execute
	bool UseMeteorSkill(const FVector& TargetLocation);
	bool UseIceLanceSkill(const FVector& TargetLocation);
	bool UseThunderSkill(const FVector& TargetLocation);
	bool UseWindSkill(const FVector& TargetLocation);
	bool UseFrostFieldSkill(const FVector& TargetLocation);
	bool UseMeteorRainSkill(const FVector& TargetLocation);

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
	float FrostFieldIdentityGain = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float BasicAttackIdentityGain = 2.0f;

	// =======================================================================================
	// Identity Gain - Additional Per Extra Target

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float FrostFieldAdditionalIdentityGainPerTarget = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float BasicAttackAdditionalIdentityGainPerTarget = 0.0f;

	// =======================================================================================
	// Identity Gain - Max Per Hit Unit

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Identity")
	float FrostFieldMaxIdentityGainPerTick = 1.0f;

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

	FLPlayerSkillTuning GetSkillTuning(ELPlayerSkillID SkillID) const;
	FLPlayerSkillTuning GetFallbackSkillTuning(ELPlayerSkillID SkillID) const;
	FLPlayerSkillCombatTuning GetSkillCombatTuning(ELPlayerSkillID SkillID) const;
	FLPlayerSkillCombatTuning GetFallbackSkillCombatTuning(ELPlayerSkillID SkillID) const;
	FLPlayerSkillIdentityTuning GetSkillIdentityTuning(ELPlayerSkillID SkillID) const;
	FLPlayerSkillIdentityTuning GetFallbackSkillIdentityTuning(ELPlayerSkillID SkillID) const;
	FLPlayerIceLanceTuning GetSkillIceLanceTuning() const;
	FLPlayerIceLanceTuning GetFallbackSkillIceLanceTuning() const;
	const ULPlayerSkillDataAsset* GetSkillDataAsset(ELPlayerSkillID SkillID) const;
	UClass* GetSkillActorClass(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillMainNiagara(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillCastStartNiagara(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillWarningNiagara(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillImpactNiagara(ELPlayerSkillID SkillID) const;
	float GetSkillCastStartEffectHeightOffset(ELPlayerSkillID SkillID) const;
	void StartSkillCooldown(ELPlayerSkillID SkillID);
	float GetSkillCooldownDuration(ELPlayerSkillID SkillID) const;
	float GetSkillLockDuration(ELPlayerSkillID SkillID) const;
	FTimerHandle* GetSkillCooldownTimerHandle(ELPlayerSkillID SkillID);
	const FTimerHandle* GetSkillCooldownTimerHandle(ELPlayerSkillID SkillID) const;
	void StartSkillLock(ELPlayerSkillID SkillID);
	void PlaySkillMontage(ELPlayerSkillID SkillID);
	void StopActiveSkillMontage();
	void ResetSkillCooldown(ELPlayerSkillID SkillID);
	
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
	bool CommitSkillCostAndCooldown(ELPlayerSkillID SkillID);
	
	
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
	// Skill Data
	// 지정된 Database에서 스킬 DataAsset을 찾으면 이 값이 우선한다.
	// Database가 비어 있거나 해당 SkillID가 없으면 아래 legacy UPROPERTY 값을 fallback으로 사용한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Data")
	TObjectPtr<ULPlayerSkillDatabase> SkillDatabase;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveSkillMontage;
	
	// =======================================================================================
	// Basic Attack
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	TObjectPtr<UNiagaraSystem> BasicAttackNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|BasicAttack")
	TObjectPtr<UAnimMontage> BasicAttackMontage;
	
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

	FVector PendingBasicAttackDirection = FVector::ForwardVector;
	bool bBasicAttackHitTriggered = false;
	
	void ApplyBasicAttackDamage(const FVector& AttackDirection);
	
	
	// =======================================================================================
	// Dash Blink
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	float BlinkDuration = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	TObjectPtr<UNiagaraSystem> BlinkStartEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Blink")
	TObjectPtr<UNiagaraSystem> BlinkEndEffect;

	void ApplyWindDamage(const FVector& AttackDirection);

	// =======================================================================================
	// Skill Cooldown - 실제 스킬 기준 재사용 대기시간

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float FrostFieldCooldown = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	float MeteorRainCooldown = 10.0f;
	
	// =======================================================================================
	// Skill Mana Cost
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Mana")
	float FrostFieldManaCost = 900.f;

	// =======================================================================================
	// Frost Field

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	TSubclassOf<ALGroundAreaSkillActor> FrostFieldActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	TObjectPtr<UNiagaraSystem> FrostFieldNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	float FrostFieldDamage = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	float FrostFieldRadius = 320.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	float FrostFieldDuration = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	float FrostFieldTickInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	float FrostFieldSkillLockDuration = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|FrostField")
	float FrostFieldSpawnHeightOffset = 0.0f;
	
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
	FTimerHandle FrostFieldCooldownTimerHandle;
	FTimerHandle MeteorRainCooldownTimerHandle;
	
	// 캐스팅 타이머
	FTimerHandle CastTimerHandle;
	
	bool bIsCasting = false;
	
	ELPlayerSkillSlot CastingSkillSlot = ELPlayerSkillSlot::Q;
	ELPlayerSkillID CastingSkillID = ELPlayerSkillID::None;
	
	FVector CastingTargetLocation = FVector::ZeroVector;
	
	float CurrentCastDuration = 0.0f;
};
