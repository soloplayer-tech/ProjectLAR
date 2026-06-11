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
class UNiagaraComponent;
class UAnimMontage;
class UStaticMeshComponent;
class USoundBase;
class UTexture2D;
class ULInventoryComponent;

UCLASS()
class PROJECTLAR_API ALPlayerCharacter : public ALPlayerCharacterBase
{
	GENERATED_BODY()
	
public:
	ALPlayerCharacter();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	ULInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	void RefreshEquippedWeaponVisual();

	UFUNCTION(BlueprintPure, Category = "Inventory|Weapon|Visual")
	UStaticMeshComponent* GetWeaponVisualStaticMeshComponent() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Weapon|Visual")
	UNiagaraComponent* GetWeaponVisualNiagaraComponent() const;
	
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<ULInventoryComponent> InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|Visual")
	FName WeaponVisualStaticMeshComponentName = TEXT("StaticMesh_GEN_VARIABLE");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Weapon|Visual")
	FName WeaponVisualNiagaraComponentName = TEXT("Niagara_GEN_VARIABLE");

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
	
	void RecoverHP(float Amount);
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

	UFUNCTION(BlueprintCallable, Category = "Skill|Audio")
	void PlaySkillImpactSound(ELPlayerSkillID SkillID, const FVector& Location) const;
	
	
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
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
	UStaticMeshComponent* FindWeaponVisualStaticMeshComponent() const;
	UNiagaraComponent* FindWeaponVisualNiagaraComponent() const;
	
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (ClampMin = "0.0"))
	float WeaponAttackPowerDamageRate = 0.001f;
	
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
	FLPlayerSkillCombatTuning GetSkillCombatTuning(ELPlayerSkillID SkillID) const;
	FLPlayerSkillIdentityTuning GetSkillIdentityTuning(ELPlayerSkillID SkillID) const;
	FLPlayerIceLanceTuning GetSkillIceLanceTuning() const;
	const ULPlayerSkillDataAsset* GetSkillDataAsset(ELPlayerSkillID SkillID) const;
	UClass* GetSkillActorClass(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillMainNiagara(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillCastStartNiagara(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillWarningNiagara(ELPlayerSkillID SkillID) const;
	UNiagaraSystem* GetSkillImpactNiagara(ELPlayerSkillID SkillID) const;
	float GetSkillCastStartEffectHeightOffset(ELPlayerSkillID SkillID) const;
	void ValidateSkillDataSetup() const;
	bool ValidateSkillDataAsset(
		ELPlayerSkillID SkillID,
		const ULPlayerSkillDataAsset* SkillDataAsset
	) const;
	void StartSkillCooldown(ELPlayerSkillID SkillID);
	float GetSkillCooldownDuration(ELPlayerSkillID SkillID) const;
	float GetSkillLockDuration(ELPlayerSkillID SkillID) const;
	FTimerHandle* GetSkillCooldownTimerHandle(ELPlayerSkillID SkillID);
	const FTimerHandle* GetSkillCooldownTimerHandle(ELPlayerSkillID SkillID) const;
	void StartSkillLock(ELPlayerSkillID SkillID);
	void PlaySkillMontage(ELPlayerSkillID SkillID);
	void PlaySkillStartSound(ELPlayerSkillID SkillID) const;
	void PlaySkillCastStartSound(ELPlayerSkillID SkillID) const;
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
	// 지정된 Database에서 스킬 DataAsset을 찾아 스킬 수치와 연출 데이터를 읽는다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Data")
	TObjectPtr<ULPlayerSkillDatabase> SkillDatabase;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveSkillMontage;
	
	// =======================================================================================
	// Basic Attack
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Audio")
	TObjectPtr<USoundBase> DashStartSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Audio", meta = (ClampMin = "0.0"))
	float DashStartSoundVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Audio", meta = (ClampMin = "0.0"))
	float DashStartSoundPitch = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Audio")
	TObjectPtr<USoundBase> DashEndSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Audio", meta = (ClampMin = "0.0"))
	float DashEndSoundVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Dash|Audio", meta = (ClampMin = "0.0"))
	float DashEndSoundPitch = 1.0f;

	void ApplyWindDamage(const FVector& AttackDirection);

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
