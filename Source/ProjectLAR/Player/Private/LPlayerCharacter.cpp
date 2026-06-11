// Fill out your copyright notice in the Description page of Project Settings.

#include "LPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
// #include "DrawDebugHelpers.h"
// #include "Components/SlateWrapperTypes.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectLAR/Item/Public/LInventoryComponent.h"
#include "ProjectLAR/Save/Public/LPlayerSaveGames.h"
#include "ProjectLAR/Skill/Public/LIceLanceActor.h"
#include "Sound/SoundBase.h"

ALPlayerCharacter::ALPlayerCharacter()
{
	bBlink = true;
	InventoryComponent = CreateDefaultSubobject<ULInventoryComponent>(TEXT("InventoryComponent"));
}

ULInventoryComponent* ALPlayerCharacter::GetInventoryComponent() const
{
	return InventoryComponent;
}

UStaticMeshComponent* ALPlayerCharacter::GetWeaponVisualStaticMeshComponent() const
{
	return FindWeaponVisualStaticMeshComponent();
}

UNiagaraComponent* ALPlayerCharacter::GetWeaponVisualNiagaraComponent() const
{
	return FindWeaponVisualNiagaraComponent();
}

void ALPlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RefreshEquippedWeaponVisual();
}

void ALPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = MaxHP;
	CurrentMana = MaxMana;
	CurrentIdentityGauge = 0.f;

	LoadEquippedSkillSlots();
	ValidateSkillDataSetup();
	RefreshEquippedWeaponVisual();
}

void ALPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	RegenerateMana(DeltaSeconds);
}

void ALPlayerCharacter::RefreshEquippedWeaponVisual()
{
	UStaticMeshComponent* WeaponMeshComp =
		FindWeaponVisualStaticMeshComponent();
	UNiagaraComponent* WeaponNiagaraComp =
		FindWeaponVisualNiagaraComponent();

	FLInventorySlot EquippedWeaponSlot;
	const bool bHasEquippedWeapon =
		InventoryComponent
		&& InventoryComponent->GetEquippedWeaponSlot(EquippedWeaponSlot)
		&& EquippedWeaponSlot.ItemData;

	if (WeaponMeshComp)
	{
		WeaponMeshComp->SetHiddenInGame(!bHasEquippedWeapon);
		WeaponMeshComp->SetVisibility(bHasEquippedWeapon, true);
	}

	if (WeaponNiagaraComp)
	{
		WeaponNiagaraComp->SetHiddenInGame(!bHasEquippedWeapon);
		WeaponNiagaraComp->SetVisibility(bHasEquippedWeapon, true);

		if (bHasEquippedWeapon)
		{
			WeaponNiagaraComp->Activate(true);
		}
		else
		{
			WeaponNiagaraComp->Deactivate();
		}
	}
}

UStaticMeshComponent* ALPlayerCharacter::FindWeaponVisualStaticMeshComponent() const
{
	if (WeaponVisualStaticMeshComponentName.IsNone())
	{
		return nullptr;
	}

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
	{
		if (StaticMeshComponent
			&& StaticMeshComponent->GetFName() == WeaponVisualStaticMeshComponentName)
		{
			return StaticMeshComponent;
		}
	}

	return nullptr;
}

UNiagaraComponent* ALPlayerCharacter::FindWeaponVisualNiagaraComponent() const
{
	if (WeaponVisualNiagaraComponentName.IsNone())
	{
		return nullptr;
	}

	TArray<UNiagaraComponent*> NiagaraComponents;
	GetComponents<UNiagaraComponent>(NiagaraComponents);

	for (UNiagaraComponent* NiagaraComponent : NiagaraComponents)
	{
		if (NiagaraComponent
			&& NiagaraComponent->GetFName() == WeaponVisualNiagaraComponentName)
		{
			return NiagaraComponent;
		}
	}

	return nullptr;
}

float ALPlayerCharacter::GetHPRatio() const
{
	if (MaxHP <= 0.f)
	{
		return 0.f;
	}
	
	return FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f);
}

float ALPlayerCharacter::GetManaRatio() const
{
	if (MaxMana <= 0.f)
	{
		return 0.f;
	}
	
	return FMath::Clamp(CurrentMana / MaxMana, 0.f, 1.f);
}

float ALPlayerCharacter::GetIdentityRatio() const
{
	if (MaxIdentityGauge <= 0.f)
	{
		return 0.f;
	}
	
	return FMath::Clamp(CurrentIdentityGauge / MaxIdentityGauge, 0.f, 1.f);
}

float ALPlayerCharacter::GetCurrentHP() const
{
	return CurrentHP;
}

float ALPlayerCharacter::GetCurrentMana() const
{
	return CurrentMana;
}

float ALPlayerCharacter::GetCurrentIdentityGauge() const
{
	return CurrentIdentityGauge;
}

bool ALPlayerCharacter::CanSpendMana(float ManaCost) const
{
	return CurrentMana >= ManaCost;
}

bool ALPlayerCharacter::SpendMana(float ManaCost)
{
	if (ManaCost <= 0.f)
	{
		return true;
	}
	
	if (!CanSpendMana(ManaCost))
	{
		return false;
	}
	
	CurrentMana = FMath::Clamp(CurrentMana - ManaCost, 0.f, MaxMana);
	
	return true;
}

void ALPlayerCharacter::RecoverHP(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.0f, MaxHP);
}

void ALPlayerCharacter::RecoverMana(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}
	
	CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.f, MaxMana);
}

void ALPlayerCharacter::RegenerateMana(float DeltaTime)
{
	if (ManaRegenPerSecond <= 0.f)
	{
		return;
	}
	
	if (CurrentMana >= MaxMana)
	{
		return;
	}
	
	RecoverMana(ManaRegenPerSecond * DeltaTime);
}

void ALPlayerCharacter::AddIdentityGauge(float Amount)
{
	if (bIdentityActive)
	{
		return;
	}

	if (Amount <= 0.0f)
	{
		return;
	}

	CurrentIdentityGauge = FMath::Clamp(
		CurrentIdentityGauge + Amount,
		0.0f,
		MaxIdentityGauge
	);
}

float ALPlayerCharacter::GetIdentityGainBySkill(ELPlayerSkillID SkillID) const
{
	return GetSkillIdentityTuning(SkillID).BaseGain;
}

float ALPlayerCharacter::GetIdentityAdditionalGainBySkill(ELPlayerSkillID SkillID) const
{
	return GetSkillIdentityTuning(SkillID).AdditionalGainPerTarget;
}

float ALPlayerCharacter::GetIdentityMaxGainBySkill(ELPlayerSkillID SkillID) const
{
	return GetSkillIdentityTuning(SkillID).MaxGainPerEvent;
}

float ALPlayerCharacter::GetIdentityGainByHitCount(
	ELPlayerSkillID SkillID,
	int32 HitCount
) const
{
	if (HitCount <= 0)
	{
		return 0.0f;
	}

	const float BaseGain = GetIdentityGainBySkill(SkillID);
	const float AdditionalGain = GetIdentityAdditionalGainBySkill(SkillID);
	const float MaxGain = GetIdentityMaxGainBySkill(SkillID);
	const int32 AdditionalHitCount = FMath::Max(0, HitCount - 1);
	const float CalculatedGain = BaseGain + AdditionalGain * AdditionalHitCount;

	if (MaxGain <= 0.0f)
	{
		return CalculatedGain;
	}

	return FMath::Min(CalculatedGain, MaxGain);
}

void ALPlayerCharacter::OnSkillHitConfirmed(
	ELPlayerSkillID SkillID,
	int32 HitCount
)
{
	if (HitCount <= 0)
	{
		return;
	}

	if (bIdentityActive)
	{
		return;
	}

	const float GainAmount = GetIdentityGainByHitCount(SkillID, HitCount);

	if (GainAmount <= 0.0f)
	{
		return;
	}

	AddIdentityGauge(GainAmount);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Identity Gain: Skill=%s / HitCount=%d / Gain=%.1f / Current=%.1f"),
		*UEnum::GetValueAsString(SkillID),
		HitCount,
		GainAmount,
		CurrentIdentityGauge
	);
}

bool ALPlayerCharacter::CanActivateIdentity() const
{
	return !bIdentityActive
		&& CurrentIdentityGauge >= MaxIdentityGauge;
}

void ALPlayerCharacter::ActivateIdentity()
{
	if (!CanActivateIdentity())
	{
		return;
	}
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	
	bIdentityActive = true;

	// 게이지 소모
	CurrentIdentityGauge = 0.0f;

	// Z 발동 순간 스킬 쿨타임 초기화
	ClearAllSkillCooldowns();
	
	// 아이덴티티 버프 VFX 시작
	StartIdentityBuffVFX();
	
	BP_OnIdentityActivated();

	
	
	GetWorldTimerManager().ClearTimer(IdentityTimerHandle);

	GetWorldTimerManager().SetTimer(
		IdentityTimerHandle,
		this,
		&ALPlayerCharacter::EndIdentity,
		IdentityDuration,
		false
	);
}

void ALPlayerCharacter::EndIdentity()
{
	bIdentityActive = false;
	StopIdentityBuffVFX();
	
	BP_OnIdentityEnded();
	
	GetWorldTimerManager().ClearTimer(IdentityTimerHandle);
}

void ALPlayerCharacter::ClearAllSkillCooldowns()
{
	GetWorldTimerManager().ClearTimer(MeteorCooldownTimerHandle);
	GetWorldTimerManager().ClearTimer(IceLanceCooldownTimerHandle);
	GetWorldTimerManager().ClearTimer(ThunderCooldownTimerHandle);
	GetWorldTimerManager().ClearTimer(WindCooldownTimerHandle);
	GetWorldTimerManager().ClearTimer(FrostFieldCooldownTimerHandle);
	GetWorldTimerManager().ClearTimer(MeteorRainCooldownTimerHandle);
}


bool ALPlayerCharacter::IsIdentityActive() const
{
	return bIdentityActive;
}

bool ALPlayerCharacter::IsIdentityFull() const
{
	return CurrentIdentityGauge >= MaxIdentityGauge;
}

void ALPlayerCharacter::Dash(const FVector& DashDirection)
{
	if (!CanDash())
	{
		return;
	}

	if (DashStartSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DashStartSound,
			GetActorLocation(),
			DashStartSoundVolume,
			DashStartSoundPitch
		);
	}
	
	if (bBlink)
	{
		if (BlinkStartEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				BlinkStartEffect,
				GetActorLocation(),
				GetActorRotation()
			);
		}

		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetHiddenInGame(true, true);
		}

		GetWorldTimerManager().ClearTimer(BlinkTimerHandle);

		if (BlinkDuration > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				BlinkTimerHandle,
				this,
				&ALPlayerCharacter::EndBlink,
				BlinkDuration,
				false
			);
		}
		else
		{
			EndBlink();
		}
	}
	
	Super::Dash(DashDirection);
}

void ALPlayerCharacter::EndBlink()
{
	if (BlinkEndEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BlinkEndEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (DashEndSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DashEndSound,
			GetActorLocation(),
			DashEndSoundVolume,
			DashEndSoundPitch
		);
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetHiddenInGame(false, true);
	}

	RefreshEquippedWeaponVisual();
}

void ALPlayerCharacter::StartIdentityBuffVFX()
{
	if (!IdentityBuffVFX)
	{
		return;
	}

	// 이미 켜져 있으면 중복 생성 방지
	if (ActiveIdentityBuffVFXComponent)
	{
		return;
	}

	ActiveIdentityBuffVFXComponent =
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			IdentityBuffVFX,
			GetRootComponent(),
			NAME_None,
			IdentityBuffVFXLocationOffset,
			IdentityBuffVFXRotationOffset,
			EAttachLocation::KeepRelativeOffset,
			true
			
		);
}


void ALPlayerCharacter::StopIdentityBuffVFX()
{
	if (!ActiveIdentityBuffVFXComponent)
	{
		return;
	}

	ActiveIdentityBuffVFXComponent->Deactivate();
	ActiveIdentityBuffVFXComponent = nullptr;
}

bool ALPlayerCharacter::PrepareActionDirection(
	const FVector& TargetLocation,
	FVector& OutDirection,
	FRotator& OutRotation
)
{
	OutDirection = TargetLocation - GetActorLocation();
	OutDirection.Z = 0.0f;

	if (OutDirection.IsNearlyZero())
	{
		return false;
	}

	OutDirection.Normalize();

	OutRotation = OutDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			OutRotation.Yaw,
			0.0f
		)
	);

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	return true;
}

void ALPlayerCharacter::BasicAttack(const FVector& TargetLocation)
{
	const ULPlayerSkillDataAsset* BasicAttackDataAsset =
		GetSkillDataAsset(ELPlayerSkillID::BasicAttack);

	if (!BasicAttackDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("BasicAttack failed: DA_BasicAttack is not set."));
		SetCurrentActionState(ELPlayerActionState::Idle);
		return;
	}

	UAnimMontage* BasicAttackMontage = BasicAttackDataAsset->SkillMontage.Get();

	if (!BasicAttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("BasicAttack failed: DA_BasicAttack SkillMontage is not set."));
		SetCurrentActionState(ELPlayerActionState::Idle);
		return;
	}

	FVector AttackDirection;
	FRotator AttackRotation;

	if (!PrepareActionDirection(TargetLocation, AttackDirection, AttackRotation))
	{
		return;
	}

	const float MontageDuration = PlayAnimMontage(BasicAttackMontage);

	if (MontageDuration <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BasicAttack failed: BasicAttackMontage could not be played."));
		SetCurrentActionState(ELPlayerActionState::Idle);
		return;
	}
	
	SetCurrentActionState(ELPlayerActionState::BasicAttack);
	PendingBasicAttackDirection = AttackDirection;
	bBasicAttackHitTriggered = false;

	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::BasicAttack);
	
	FVector SpawnLocation =
		GetActorLocation()
		+ AttackDirection * CombatTuning.ForwardOffset;

	SpawnLocation.Z += CombatTuning.HeightOffset;
	
	if (UNiagaraSystem* BasicAttackNiagara =
		GetSkillMainNiagara(ELPlayerSkillID::BasicAttack))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BasicAttackNiagara,
			SpawnLocation,
			AttackRotation
		);
	}
	
	GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);

	GetWorldTimerManager().SetTimer(
		BasicAttackTimerHandle,
		this,
		&ALPlayerCharacter::EndBasicAttack,
		MontageDuration,
		false
	);
}

void ALPlayerCharacter::TriggerBasicAttackHit()
{
	if (GetCurrentActionState() != ELPlayerActionState::BasicAttack)
	{
		return;
	}

	if (bBasicAttackHitTriggered)
	{
		return;
	}

	bBasicAttackHitTriggered = true;
	ApplyBasicAttackDamage(PendingBasicAttackDirection);
}

void ALPlayerCharacter::EndBasicAttack()
{
	bBasicAttackHitTriggered = false;
	SetCurrentActionState(ELPlayerActionState::Idle);
}

void ALPlayerCharacter::EndSkill()
{
	StopActiveSkillMontage();
	SetCurrentActionState(ELPlayerActionState::Idle);
}

// =======================================================================================
// Skill Tuning - SkillID 기준
FLPlayerSkillTuning ALPlayerCharacter::GetSkillTuning(ELPlayerSkillID SkillID) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		return SkillDataAsset->Tuning;
	}

	return FLPlayerSkillTuning();
}

const ULPlayerSkillDataAsset* ALPlayerCharacter::GetSkillDataAsset(ELPlayerSkillID SkillID) const
{
	if (!SkillDatabase)
	{
		return nullptr;
	}

	return SkillDatabase->FindSkillDataAsset(SkillID);
}

void ALPlayerCharacter::PlaySkillStartSound(ELPlayerSkillID SkillID) const
{
	const ULPlayerSkillDataAsset* SkillDataAsset =
		GetSkillDataAsset(SkillID);

	if (!SkillDataAsset || !SkillDataAsset->AudioTuning.StartSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this,
		SkillDataAsset->AudioTuning.StartSound,
		GetActorLocation(),
		SkillDataAsset->AudioTuning.StartSoundVolume,
		SkillDataAsset->AudioTuning.StartSoundPitch
	);
}

void ALPlayerCharacter::PlaySkillCastStartSound(ELPlayerSkillID SkillID) const
{
	const ULPlayerSkillDataAsset* SkillDataAsset =
		GetSkillDataAsset(SkillID);

	if (!SkillDataAsset || !SkillDataAsset->AudioTuning.CastStartSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this,
		SkillDataAsset->AudioTuning.CastStartSound,
		GetActorLocation(),
		SkillDataAsset->AudioTuning.CastStartSoundVolume,
		SkillDataAsset->AudioTuning.CastStartSoundPitch
	);
}

void ALPlayerCharacter::PlaySkillImpactSound(
	ELPlayerSkillID SkillID,
	const FVector& Location
) const
{
	const ULPlayerSkillDataAsset* SkillDataAsset =
		GetSkillDataAsset(SkillID);

	if (!SkillDataAsset || !SkillDataAsset->AudioTuning.ImpactSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this,
		SkillDataAsset->AudioTuning.ImpactSound,
		Location,
		SkillDataAsset->AudioTuning.ImpactSoundVolume,
		SkillDataAsset->AudioTuning.ImpactSoundPitch
	);
}

FLPlayerSkillCombatTuning ALPlayerCharacter::GetSkillCombatTuning(
	ELPlayerSkillID SkillID
) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->CombatTuning.bOverrideCombatValues)
		{
			return SkillDataAsset->CombatTuning;
		}
	}

	return FLPlayerSkillCombatTuning();
}

FLPlayerSkillIdentityTuning ALPlayerCharacter::GetSkillIdentityTuning(
	ELPlayerSkillID SkillID
) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->IdentityTuning.bOverrideIdentityGain)
		{
			return SkillDataAsset->IdentityTuning;
		}
	}

	return FLPlayerSkillIdentityTuning();
}

FLPlayerIceLanceTuning ALPlayerCharacter::GetSkillIceLanceTuning() const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset =
		GetSkillDataAsset(ELPlayerSkillID::IceLance))
	{
		if (SkillDataAsset->IceLanceTuning.bOverrideIceLanceValues)
		{
			return SkillDataAsset->IceLanceTuning;
		}
	}

	return FLPlayerIceLanceTuning();
}

UClass* ALPlayerCharacter::GetSkillActorClass(ELPlayerSkillID SkillID) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->SkillActorClass)
		{
			return SkillDataAsset->SkillActorClass.Get();
		}
	}

	switch (SkillID)
	{
	case ELPlayerSkillID::FrostField:
	case ELPlayerSkillID::Wind:
	case ELPlayerSkillID::MeteorRain:
	case ELPlayerSkillID::BasicAttack:
	case ELPlayerSkillID::None:
	default:
		return nullptr;
	}
}

UNiagaraSystem* ALPlayerCharacter::GetSkillMainNiagara(
	ELPlayerSkillID SkillID
) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->VisualTuning.MainNiagara)
		{
			return SkillDataAsset->VisualTuning.MainNiagara.Get();
		}
	}

	switch (SkillID)
	{
	case ELPlayerSkillID::FrostField:
	case ELPlayerSkillID::Meteor:
	case ELPlayerSkillID::Thunder:
	case ELPlayerSkillID::MeteorRain:
	case ELPlayerSkillID::BasicAttack:
	case ELPlayerSkillID::None:
	default:
		return nullptr;
	}
}

UNiagaraSystem* ALPlayerCharacter::GetSkillCastStartNiagara(
	ELPlayerSkillID SkillID
) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->VisualTuning.CastStartNiagara)
		{
			return SkillDataAsset->VisualTuning.CastStartNiagara.Get();
		}
	}

	switch (SkillID)
	{
	case ELPlayerSkillID::IceLance:
	case ELPlayerSkillID::Wind:
	case ELPlayerSkillID::FrostField:
	case ELPlayerSkillID::MeteorRain:
	case ELPlayerSkillID::BasicAttack:
	case ELPlayerSkillID::None:
	default:
		return nullptr;
	}
}

UNiagaraSystem* ALPlayerCharacter::GetSkillWarningNiagara(
	ELPlayerSkillID SkillID
) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->VisualTuning.WarningNiagara)
		{
			return SkillDataAsset->VisualTuning.WarningNiagara.Get();
		}
	}

	return nullptr;
}

UNiagaraSystem* ALPlayerCharacter::GetSkillImpactNiagara(
	ELPlayerSkillID SkillID
) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->VisualTuning.ImpactNiagara)
		{
			return SkillDataAsset->VisualTuning.ImpactNiagara.Get();
		}
	}

	return nullptr;
}

float ALPlayerCharacter::GetSkillCastStartEffectHeightOffset(
	ELPlayerSkillID SkillID
) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (SkillDataAsset->VisualTuning.CastStartNiagara)
		{
			return SkillDataAsset->VisualTuning.CastStartHeightOffset;
		}
	}

	switch (SkillID)
	{
	case ELPlayerSkillID::IceLance:
	case ELPlayerSkillID::Wind:
	case ELPlayerSkillID::FrostField:
	case ELPlayerSkillID::MeteorRain:
	case ELPlayerSkillID::BasicAttack:
	case ELPlayerSkillID::None:
	default:
		return 0.0f;
	}
}

void ALPlayerCharacter::ValidateSkillDataSetup() const
{
	if (!SkillDatabase)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillData Validation: SkillDatabase is not set."));
		return;
	}

	const TArray<ELPlayerSkillID> RequiredSkillIDs =
	{
		ELPlayerSkillID::BasicAttack,
		ELPlayerSkillID::Meteor,
		ELPlayerSkillID::IceLance,
		ELPlayerSkillID::Thunder,
		ELPlayerSkillID::Wind,
		ELPlayerSkillID::FrostField
	};

	bool bAllValid = true;

	for (const ELPlayerSkillID SkillID : RequiredSkillIDs)
	{
		bAllValid &= ValidateSkillDataAsset(SkillID, GetSkillDataAsset(SkillID));
	}

	if (bAllValid)
	{
		UE_LOG(LogTemp, Log, TEXT("SkillData Validation: all required player skills look valid."));
	}
}

bool ALPlayerCharacter::ValidateSkillDataAsset(
	ELPlayerSkillID SkillID,
	const ULPlayerSkillDataAsset* SkillDataAsset
) const
{
	bool bValid = true;

	const FString SkillName = UEnum::GetValueAsString(SkillID);

	const auto Warn =
		[&bValid, &SkillName](const FString& Message)
		{
			bValid = false;
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("SkillData Validation: %s / %s"),
				*SkillName,
				*Message
			);
		};

	if (!SkillDataAsset)
	{
		Warn(TEXT("DataAsset is missing from SkillDatabase."));
		return false;
	}

	const FLPlayerSkillTuning& Tuning = SkillDataAsset->Tuning;
	const FLPlayerSkillCombatTuning& CombatTuning = SkillDataAsset->CombatTuning;
	const FLPlayerSkillIdentityTuning& IdentityTuning = SkillDataAsset->IdentityTuning;

	if (Tuning.CastDuration > 0.0f && !Tuning.bRequiresCasting)
	{
		Warn(TEXT("CastDuration is set, but bRequiresCasting is false."));
	}

	if (Tuning.bRequiresCasting && Tuning.CastDuration <= 0.0f)
	{
		Warn(TEXT("bRequiresCasting is true, but CastDuration is not positive."));
	}

	if (!CombatTuning.bOverrideCombatValues)
	{
		Warn(TEXT("CombatTuning override is off; combat values will be zero."));
	}

	if (!IdentityTuning.bOverrideIdentityGain)
	{
		Warn(TEXT("IdentityTuning override is off; identity gain will be zero."));
	}

	const auto ValidateActorClass =
		[&Warn, SkillDataAsset](const UClass* RequiredClass)
		{
			UClass* ActorClass = SkillDataAsset->SkillActorClass.Get();

			if (!ActorClass)
			{
				Warn(TEXT("SkillActorClass is not set."));
				return;
			}

			if (!ActorClass->IsChildOf(RequiredClass))
			{
				Warn(FString::Printf(
					TEXT("SkillActorClass must inherit from %s."),
					*RequiredClass->GetName()
				));
			}
		};

	const auto ValidateSphereDamage =
		[&Warn, &CombatTuning]()
		{
			if (CombatTuning.Damage <= 0.0f)
			{
				Warn(TEXT("Damage must be positive."));
			}

			if (CombatTuning.Radius <= 0.0f)
			{
				Warn(TEXT("Radius must be positive."));
			}
		};

	const auto ValidateBoxDamage =
		[&Warn, &CombatTuning]()
		{
			if (CombatTuning.Damage <= 0.0f)
			{
				Warn(TEXT("Damage must be positive."));
			}

			if (CombatTuning.BoxHalfExtent.IsNearlyZero())
			{
				Warn(TEXT("BoxHalfExtent is zero."));
			}
		};

	switch (SkillID)
	{
	case ELPlayerSkillID::BasicAttack:
		if (!SkillDataAsset->SkillMontage)
		{
			Warn(TEXT("SkillMontage is required for BasicAttack."));
		}

		ValidateBoxDamage();
		break;

	case ELPlayerSkillID::Meteor:
		ValidateActorClass(ALMeteorActor::StaticClass());
		ValidateSphereDamage();

		if (CombatTuning.Duration <= 0.0f)
		{
			Warn(TEXT("Duration must be positive for Meteor fall time."));
		}
		break;

	case ELPlayerSkillID::IceLance:
		ValidateActorClass(ALIceLanceActor::StaticClass());
		ValidateSphereDamage();

		if (!SkillDataAsset->IceLanceTuning.bOverrideIceLanceValues)
		{
			Warn(TEXT("IceLanceTuning override is off."));
		}

		if (SkillDataAsset->IceLanceTuning.Count <= 0)
		{
			Warn(TEXT("IceLance Count must be positive."));
		}

		if (SkillDataAsset->IceLanceTuning.TravelDuration <= 0.0f)
		{
			Warn(TEXT("IceLance TravelDuration must be positive."));
		}
		break;

	case ELPlayerSkillID::Thunder:
		ValidateActorClass(ALThunderActor::StaticClass());
		ValidateSphereDamage();

		if (CombatTuning.Count <= 0)
		{
			Warn(TEXT("Count must be positive for Thunder strikes."));
		}

		if (CombatTuning.SpawnRadius <= 0.0f)
		{
			Warn(TEXT("SpawnRadius must be positive for Thunder."));
		}

		if (CombatTuning.TickInterval <= 0.0f)
		{
			Warn(TEXT("TickInterval must be positive for Thunder."));
		}
		break;

	case ELPlayerSkillID::Wind:
		ValidateBoxDamage();
		break;

	case ELPlayerSkillID::FrostField:
		ValidateSphereDamage();

		if (CombatTuning.Duration <= 0.0f)
		{
			Warn(TEXT("Duration must be positive for FrostField."));
		}

		if (CombatTuning.TickInterval <= 0.0f)
		{
			Warn(TEXT("TickInterval must be positive for FrostField."));
		}

		if (UClass* ActorClass = SkillDataAsset->SkillActorClass.Get())
		{
			if (!ActorClass->IsChildOf(ALGroundAreaSkillActor::StaticClass()))
			{
				Warn(TEXT("FrostField SkillActorClass must inherit from ALGroundAreaSkillActor."));
			}
		}
		break;

	case ELPlayerSkillID::MeteorRain:
	case ELPlayerSkillID::None:
	default:
		break;
	}

	return bValid;
}

float ALPlayerCharacter::GetSkillManaCost(ELPlayerSkillID SkillID) const
{
	return GetSkillTuning(SkillID).ManaCost;
}
// =======================================================================================
// Skill Cooldown - SkillID 기준

bool ALPlayerCharacter::CanUseSkillSlot(ELPlayerSkillSlot SkillSlot) const
{
	const ELPlayerSkillID EquippedSkillID = GetEquippedSkillID(SkillSlot);

	return CanUseSkillID(EquippedSkillID);
}

bool ALPlayerCharacter::CanUseSkillID(ELPlayerSkillID SkillID) const
{
	if (SkillID == ELPlayerSkillID::None)
	{
		return false;
	}

	return CanUseSkill()
		&& !IsSkillIDOnCooldown(SkillID);
}

bool ALPlayerCharacter::IsSkillIDOnCooldown(ELPlayerSkillID SkillID) const
{
	if (!GetWorld())
	{
		return false;
	}

	const FTimerHandle* CooldownTimerHandle =
		GetSkillCooldownTimerHandle(SkillID);

	if (!CooldownTimerHandle)
	{
		return false;
	}

	return GetWorldTimerManager().IsTimerActive(*CooldownTimerHandle);
}

float ALPlayerCharacter::GetSkillIDCooldownRemaining(ELPlayerSkillID SkillID) const
{
	if (!GetWorld())
	{
		return 0.0f;
	}

	const FTimerHandle* CooldownTimerHandle =
		GetSkillCooldownTimerHandle(SkillID);

	if (!CooldownTimerHandle)
	{
		return 0.0f;
	}

	if (!GetWorldTimerManager().IsTimerActive(*CooldownTimerHandle))
	{
		return 0.0f;
	}

	return FMath::Max(
		0.0f,
		GetWorldTimerManager().GetTimerRemaining(*CooldownTimerHandle)
	);
}

float ALPlayerCharacter::GetSkillIDCooldownRatio(ELPlayerSkillID SkillID) const
{
	const float CooldownDuration = GetSkillCooldownDuration(SkillID);

	if (CooldownDuration <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(
		GetSkillIDCooldownRemaining(SkillID) / CooldownDuration,
		0.0f,
		1.0f
	);
}

float ALPlayerCharacter::GetSkillCooldownDuration(ELPlayerSkillID SkillID) const
{
	return GetSkillTuning(SkillID).CooldownDuration;
}

float ALPlayerCharacter::GetSkillLockDuration(ELPlayerSkillID SkillID) const
{
	return GetSkillTuning(SkillID).SkillLockDuration;
}

FTimerHandle* ALPlayerCharacter::GetSkillCooldownTimerHandle(ELPlayerSkillID SkillID)
{
	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return &MeteorCooldownTimerHandle;

	case ELPlayerSkillID::IceLance:
		return &IceLanceCooldownTimerHandle;

	case ELPlayerSkillID::Thunder:
		return &ThunderCooldownTimerHandle;

	case ELPlayerSkillID::Wind:
		return &WindCooldownTimerHandle;

	case ELPlayerSkillID::FrostField:
		return &FrostFieldCooldownTimerHandle;

	case ELPlayerSkillID::MeteorRain:
		return &MeteorRainCooldownTimerHandle;

	case ELPlayerSkillID::None:
	default:
		return nullptr;
	}
}

const FTimerHandle* ALPlayerCharacter::GetSkillCooldownTimerHandle(ELPlayerSkillID SkillID) const
{
	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return &MeteorCooldownTimerHandle;

	case ELPlayerSkillID::IceLance:
		return &IceLanceCooldownTimerHandle;

	case ELPlayerSkillID::Thunder:
		return &ThunderCooldownTimerHandle;

	case ELPlayerSkillID::Wind:
		return &WindCooldownTimerHandle;

	case ELPlayerSkillID::FrostField:
		return &FrostFieldCooldownTimerHandle;

	case ELPlayerSkillID::MeteorRain:
		return &MeteorRainCooldownTimerHandle;

	case ELPlayerSkillID::None:
	default:
		return nullptr;
	}
}

void ALPlayerCharacter::StartSkillCooldown(ELPlayerSkillID SkillID)
{
	if (!GetWorld())
	{
		return;
	}

	const float CooldownDuration = GetSkillCooldownDuration(SkillID);

	if (CooldownDuration <= 0.0f)
	{
		return;
	}

	FTimerHandle* CooldownTimerHandle =
		GetSkillCooldownTimerHandle(SkillID);

	if (!CooldownTimerHandle)
	{
		return;
	}

	FTimerDelegate CooldownFinishedDelegate;
	CooldownFinishedDelegate.BindUObject(
		this,
		&ALPlayerCharacter::ResetSkillCooldown,
		SkillID
	);

	GetWorldTimerManager().ClearTimer(*CooldownTimerHandle);
	GetWorldTimerManager().SetTimer(
		*CooldownTimerHandle,
		CooldownFinishedDelegate,
		CooldownDuration,
		false
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Skill Cooldown Start: %s / %.1f"),
		*UEnum::GetValueAsString(SkillID),
		CooldownDuration
	);
}

void ALPlayerCharacter::ResetSkillCooldown(ELPlayerSkillID SkillID)
{
}

// =======================================================================================
// Skill Equip

ELPlayerSkillID ALPlayerCharacter::GetEquippedSkillID(ELPlayerSkillSlot SkillSlot) const
{
	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		return QSlotSkill;

	case ELPlayerSkillSlot::W:
		return WSlotSkill;

	case ELPlayerSkillSlot::E:
		return ESlotSkill;

	case ELPlayerSkillSlot::R:
		return RSlotSkill;

	case ELPlayerSkillSlot::V:
		return VSlotSkill;
		
	case ELPlayerSkillSlot::A:
		return ASlotSkill;
		
	case ELPlayerSkillSlot::S:
		return SSlotSkill;
		
	case ELPlayerSkillSlot::D:
		return DSlotSkill;
		
	case ELPlayerSkillSlot::F:
		return FSlotSkill;

	default:
		return ELPlayerSkillID::None;
	}
}

void ALPlayerCharacter::SetEquippedSkillID(
	ELPlayerSkillSlot SkillSlot,
	ELPlayerSkillID SkillID)
{
	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		QSlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::W:
		WSlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::E:
		ESlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::R:
		RSlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::V:
		VSlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::A:
		ASlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::S:
		SSlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::D:
		DSlotSkill = SkillID;
		break;
		
	case ELPlayerSkillSlot::F:
		FSlotSkill = SkillID;
		break;
		
		
	default:
		break;
	}
}

void ALPlayerCharacter::EquipSkillToSlot(
	ELPlayerSkillSlot SkillSlot,
	ELPlayerSkillID SkillID
)
{
	if (SkillID == ELPlayerSkillID::None)
	{
		SetEquippedSkillID(SkillSlot, ELPlayerSkillID::None);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EquipSkillToSlot Clear / Slot: %s"),
			*UEnum::GetValueAsString(SkillSlot)
		);

		
		SaveEquippedSkillSlots();
		
		return;
	}

	const TArray<ELPlayerSkillSlot> SkillSlots =
	{
		ELPlayerSkillSlot::Q,
		ELPlayerSkillSlot::W,
		ELPlayerSkillSlot::E,
		ELPlayerSkillSlot::R,
		ELPlayerSkillSlot::A,
		ELPlayerSkillSlot::S,
		ELPlayerSkillSlot::D,
		ELPlayerSkillSlot::F,
		ELPlayerSkillSlot::V,
	};

	for (const ELPlayerSkillSlot ExistingSlot : SkillSlots)
	{
		if (ExistingSlot == SkillSlot)
		{
			continue;
		}

		if (GetEquippedSkillID(ExistingSlot) == SkillID)
		{
			SetEquippedSkillID(ExistingSlot, ELPlayerSkillID::None);
		}
	}

	SetEquippedSkillID(SkillSlot, SkillID);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("EquipSkillToSlot Success / Slot: %s / Skill: %s"),
		*UEnum::GetValueAsString(SkillSlot),
		*UEnum::GetValueAsString(SkillID)
	);
	
	SaveEquippedSkillSlots();
}

bool ALPlayerCharacter::ExecuteSkillByID(
	ELPlayerSkillID SkillID,
	const FVector& TargetLocation)
{
	bool bSkillSucceeded = false;

	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		bSkillSucceeded = UseMeteorSkill(TargetLocation);
		break;
		
	case ELPlayerSkillID::IceLance:
		bSkillSucceeded = UseIceLanceSkill(TargetLocation);
		break;
		
	case ELPlayerSkillID::Thunder:
		bSkillSucceeded = UseThunderSkill(TargetLocation);
		break;
		
	case ELPlayerSkillID::Wind:
		bSkillSucceeded = UseWindSkill(TargetLocation);
		break;

	case ELPlayerSkillID::FrostField:
		bSkillSucceeded = UseFrostFieldSkill(TargetLocation);
		break;
		
	case ELPlayerSkillID::MeteorRain:
		bSkillSucceeded = UseMeteorRainSkill(TargetLocation);
		break;

	case ELPlayerSkillID::None:
	default:
		break;
	}

	if (bSkillSucceeded && (!DoesSkillNeedCasting(SkillID) || GetSkillCastDuration(SkillID) <= 0.0f))
	{
		PlaySkillStartSound(SkillID);
		PlaySkillMontage(SkillID);
	}

	return bSkillSucceeded;
}

// =======================================================================================
// Skill Casting

bool ALPlayerCharacter::IsCasting() const
{
	return bIsCasting;
}

ELPlayerSkillID ALPlayerCharacter::GetCastingSkillID() const
{
	return CastingSkillID;
}

bool ALPlayerCharacter::DoesSkillNeedCasting(ELPlayerSkillID SkillID) const
{
	return GetSkillTuning(SkillID).bRequiresCasting;
}

float ALPlayerCharacter::GetSkillCastDuration(ELPlayerSkillID SkillID) const
{
	const float BaseDuration = GetSkillTuning(SkillID).CastDuration;

	if (bIdentityActive)
	{
		return BaseDuration * IdentityCastDurationMultiplier;
	}

	return BaseDuration;
}

void ALPlayerCharacter::StartSkillLock(ELPlayerSkillID SkillID)
{
	SetCurrentActionState(ELPlayerActionState::Skill);

	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	const float SkillLockDuration = GetSkillLockDuration(SkillID);

	if (SkillLockDuration <= 0.0f)
	{
		EndSkill();
		return;
	}

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		SkillLockDuration,
		false
	);
}

void ALPlayerCharacter::PlaySkillMontage(ELPlayerSkillID SkillID)
{
	StopActiveSkillMontage();

	const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID);

	if (!SkillDataAsset || !SkillDataAsset->SkillMontage)
	{
		return;
	}

	const float MontageDuration = PlayAnimMontage(SkillDataAsset->SkillMontage);

	if (MontageDuration <= 0.0f)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Skill montage could not be played: %s"),
			*UEnum::GetValueAsString(SkillID)
		);

		return;
	}

	ActiveSkillMontage = SkillDataAsset->SkillMontage;
}

void ALPlayerCharacter::StopActiveSkillMontage()
{
	if (!ActiveSkillMontage)
	{
		return;
	}

	StopAnimMontage(ActiveSkillMontage);
	ActiveSkillMontage = nullptr;
}

bool ALPlayerCharacter::CommitSkillCostAndCooldown(ELPlayerSkillID SkillID)
{
	const float ManaCost = GetSkillManaCost(SkillID);

	if (!CanSpendMana(ManaCost))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Skill Commit Failed: Not Enough Mana / %s"),
			*UEnum::GetValueAsString(SkillID)
		);

		return false;
	}

	if (!SpendMana(ManaCost))
	{
		return false;
	}

	StartSkillCooldown(SkillID);
	return true;
}

// ======================================================================================
// 스킬 캐스팅 시작할 때 나이아가라 시작 / 스킬 발동 후는 아님

void ALPlayerCharacter::SpawnCastStartEffect(
	ELPlayerSkillID SkillID,
	const FVector& TargetLocation
)
{
	UNiagaraSystem* CastStartEffect = GetSkillCastStartNiagara(SkillID);
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	const float HeightOffset = GetSkillCastStartEffectHeightOffset(SkillID);

	if (!CastStartEffect)
	{
		return;
	}

	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		// 메테오 캐스팅 이펙트는 플레이어 발밑
		SpawnLocation = GetActorLocation();
		SpawnLocation.Z += HeightOffset;
		SpawnRotation = GetActorRotation();
		break;

	case ELPlayerSkillID::Thunder:
		// 썬더 캐스팅 이펙트는 마우스 위치
		SpawnLocation = TargetLocation;
		SpawnLocation.Z += HeightOffset;
		SpawnRotation = FRotator::ZeroRotator;
		break;

	default:
		break;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		CastStartEffect,
		SpawnLocation,
		SpawnRotation
	);
}

void ALPlayerCharacter::StartSkillCast(
	ELPlayerSkillSlot SkillSlot,
	ELPlayerSkillID SkillID,
	const FVector& TargetLocation
)
{
	const float CastDuration = GetSkillCastDuration(SkillID);

	if (CastDuration <= 0.0f)
	{
		const float ManaCost = GetSkillManaCost(SkillID);

		if (!CanSpendMana(ManaCost))
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Enough Mana"));
			return;
		}

		const bool bSkillSucceeded =
			ExecuteSkillByID(SkillID, TargetLocation);

		if (bSkillSucceeded)
		{
			CommitSkillCostAndCooldown(SkillID);
		}

		return;
	}
	
	
	// 캐스팅 시작 시 기존 이동은 멈춘다.
	// 이후 우클릭 이동을 새로 입력하면 캐스팅이 취소되고 이동한다.
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	// 캐스팅 방향으로 캐릭터 회전
	FVector CastDirection = TargetLocation - GetActorLocation();
	CastDirection.Z = 0.0f;

	if (!CastDirection.IsNearlyZero())
	{
		CastDirection.Normalize();

		const FRotator CastRotation = CastDirection.Rotation();

		SetActorRotation(
			FRotator(
				0.0f,
				CastRotation.Yaw,
				0.0f
			)
		);
	}

	bIsCasting = true;
	CastingSkillSlot = SkillSlot;
	CastingSkillID = SkillID;
	CastingTargetLocation = TargetLocation;
	CurrentCastDuration = CastDuration;

	SetCurrentActionState(ELPlayerActionState::Casting);

	PlaySkillMontage(SkillID);
	PlaySkillCastStartSound(SkillID);
	SpawnCastStartEffect(SkillID, TargetLocation);
	
	GetWorldTimerManager().ClearTimer(CastTimerHandle);

	GetWorldTimerManager().SetTimer(
		CastTimerHandle,
		this,
		&ALPlayerCharacter::FinishSkillCast,
		CastDuration,
		false
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Casting Start: %s / %.2f"),
		*UEnum::GetValueAsString(SkillID),
		CastDuration
	);
}

void ALPlayerCharacter::FinishSkillCast()
{
	if (!bIsCasting)
	{
		return;
	}

	const ELPlayerSkillID FinishedSkillID = CastingSkillID;
	const FVector FinishedTargetLocation = CastingTargetLocation;

	bIsCasting = false;
	CastingSkillID = ELPlayerSkillID::None;
	CurrentCastDuration = 0.0f;

	GetWorldTimerManager().ClearTimer(CastTimerHandle);

	// 실제 스킬 함수가 Skill 상태를 다시 잡을 수 있게 일단 Idle로 돌린다.
	SetCurrentActionState(ELPlayerActionState::Idle);

	const float ManaCost = GetSkillManaCost(FinishedSkillID);

	if (!CanSpendMana(ManaCost))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Casting Finish Failed: Not Enough Mana / %s"),
			*UEnum::GetValueAsString(FinishedSkillID)
		);

		StopActiveSkillMontage();
		return;
	}

	const bool bSkillSucceeded =
		ExecuteSkillByID(FinishedSkillID, FinishedTargetLocation);

	if (bSkillSucceeded)
	{
		PlaySkillStartSound(FinishedSkillID);
		CommitSkillCostAndCooldown(FinishedSkillID);
	}
	else
	{
		StopActiveSkillMontage();
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Casting Finish: %s"),
		*UEnum::GetValueAsString(FinishedSkillID)
	);
}

void ALPlayerCharacter::CancelSkillCast()
{
	if (!bIsCasting)
	{
		return;
	}

	bIsCasting = false;
	CastingSkillID = ELPlayerSkillID::None;
	CurrentCastDuration = 0.0f;

	GetWorldTimerManager().ClearTimer(CastTimerHandle);
	StopActiveSkillMontage();

	if (GetCurrentActionState() == ELPlayerActionState::Casting)
	{
		SetCurrentActionState(ELPlayerActionState::Idle);
	}
}

void ALPlayerCharacter::ApplyBasicAttackDamage(const FVector& AttackDirection)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FVector NormalizedDirection = AttackDirection;
	NormalizedDirection.Z = 0.0f;

	if (!NormalizedDirection.Normalize())
	{
		return;
	}

	const FRotator DamageRotation = NormalizedDirection.Rotation();
	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::BasicAttack);

	if (CombatTuning.Damage <= 0.0f || CombatTuning.BoxHalfExtent.IsNearlyZero())
	{
		return;
	}

	FVector DamageCenter =
		GetActorLocation()
		+ NormalizedDirection * CombatTuning.BoxCenterOffset;

	DamageCenter.Z += CombatTuning.BoxHeightOffset;

	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->OverlapMultiByObjectType(
		OverlapResults,
		DamageCenter,
		DamageRotation.Quaternion(),
		ObjectQueryParams,
		FCollisionShape::MakeBox(CombatTuning.BoxHalfExtent),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	TArray<AActor*> DamagedActors;
	int32 DamagedCount = 0;

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* HitActor = Result.GetActor();

		if (!HitActor)
		{
			continue;
		}

		if (DamagedActors.Contains(HitActor))
		{
			continue;
		}

		DamagedActors.Add(HitActor);

		const bool bDamageApplied = ApplySkillDamageToActor(
			HitActor,
			CombatTuning.Damage,
			ELPlayerSkillID::BasicAttack
		);

		if (bDamageApplied)
		{
			DamagedCount++;
		}
	}

	if (DamagedCount > 0)
	{
		OnSkillHitConfirmed(ELPlayerSkillID::BasicAttack, DamagedCount);
	}
}

void ALPlayerCharacter::ApplyWindDamage(const FVector& AttackDirection)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FVector NormalizedDirection = AttackDirection;
	NormalizedDirection.Z = 0.0f;

	if (!NormalizedDirection.Normalize())
	{
		return;
	}

	const FRotator DamageRotation = NormalizedDirection.Rotation();
	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::Wind);

	FVector DamageCenter =
		GetActorLocation()
		+ NormalizedDirection * CombatTuning.BoxCenterOffset;

	DamageCenter.Z += CombatTuning.BoxHeightOffset;

	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->OverlapMultiByObjectType(
		OverlapResults,
		DamageCenter,
		DamageRotation.Quaternion(),
		ObjectQueryParams,
		FCollisionShape::MakeBox(CombatTuning.BoxHalfExtent),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	TArray<AActor*> DamagedActors;
	int32 DamagedCount = 0;

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* HitActor = Result.GetActor();

		if (!HitActor)
		{
			continue;
		}

		if (DamagedActors.Contains(HitActor))
		{
			continue;
		}

		DamagedActors.Add(HitActor);

		const bool bDamageApplied = ApplySkillDamageToActor(
			HitActor,
			CombatTuning.Damage,
			ELPlayerSkillID::Wind
		);

		if (bDamageApplied)
		{
			DamagedCount++;
		}
	}

	if (DamagedCount > 0)
	{
		OnSkillHitConfirmed(ELPlayerSkillID::Wind, DamagedCount);
	}
}

bool ALPlayerCharacter::ApplySkillDamageToActor(
	AActor* TargetActor,
	float Damage,
	ELPlayerSkillID SkillID
)
{
	if (!TargetActor)
	{
		return false;
	}

	if (TargetActor == this)
	{
		return false;
	}

	if (!TargetActor->GetClass()->ImplementsInterface(ULDamageable::StaticClass()))
	{
		return false;
	}

	const float FinalDamage = GetFinalSkillDamage(Damage, SkillID);

	ILDamageable::Execute_ReceiveSkillDamage(
		TargetActor,
		FinalDamage,
		this,
		SkillID
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Skill Damage Applied: %s / Skill: %s / Damage: %.1f"),
		*TargetActor->GetName(),
		*UEnum::GetValueAsString(SkillID),
		FinalDamage
	);

	return true;
}

float ALPlayerCharacter::GetFinalSkillDamage(
	float BaseDamage,
	ELPlayerSkillID SkillID) const
{
	float FinalDamage = BaseDamage;

	if (InventoryComponent && WeaponAttackPowerDamageRate > 0.0f)
	{
		const int32 WeaponAttackPower =
			InventoryComponent->GetEquippedWeaponAttackPower();

		if (WeaponAttackPower > 0)
		{
			FinalDamage *= 1.0f
				+ static_cast<float>(WeaponAttackPower) * WeaponAttackPowerDamageRate;
		}
	}

	if (bIdentityActive)
	{
		FinalDamage *= IdentityDamageMultiplier;
	}

	return FinalDamage;
}

FText ALPlayerCharacter::GetSkillDisplayName(ELPlayerSkillID SkillID) const
{
	if (const ULPlayerSkillDataAsset* SkillDataAsset = GetSkillDataAsset(SkillID))
	{
		if (!SkillDataAsset->DisplayName.IsEmpty())
		{
			return SkillDataAsset->DisplayName;
		}
	}

	if (const UEnum* SkillEnum = StaticEnum<ELPlayerSkillID>())
	{
		return SkillEnum->GetDisplayNameTextByValue(
			static_cast<int64>(SkillID)
		);
	}

	return FText::GetEmpty();
}

UTexture2D* ALPlayerCharacter::GetSkillIconTexture(
	ELPlayerSkillID SkillID
) const
{
	const ULPlayerSkillDataAsset* SkillDataAsset =
		GetSkillDataAsset(SkillID);

	if (!SkillDataAsset)
	{
		return nullptr;
	}

	return SkillDataAsset->IconTexture.Get();
}

float ALPlayerCharacter::GetCastRemaining() const
{
	if (!GetWorld())
	{
		return 0.0f;
	}

	if (!GetWorldTimerManager().IsTimerActive(CastTimerHandle))
	{
		return 0.0f;
	}

	return FMath::Max(
		0.0f,
		GetWorldTimerManager().GetTimerRemaining(CastTimerHandle)
	);
}

float ALPlayerCharacter::GetCastRatio() const
{
	if (!bIsCasting || CurrentCastDuration <= 0.0f)
	{
		return 0.0f;
	}

	const float Remaining = GetCastRemaining();

	return FMath::Clamp(
		1.0f - Remaining / CurrentCastDuration,
		0.0f,
		1.0f
	);
}

// =======================================================================================
// Skill Use

void ALPlayerCharacter::UseSkill(
	ELPlayerSkillSlot SkillSlot,
	const FVector& TargetLocation)
{
	const ELPlayerSkillID EquippedSkillID =
		GetEquippedSkillID(SkillSlot);
	
	if (EquippedSkillID == ELPlayerSkillID::None)
	{
		return;
	}
	
	if (!CanUseSkillID(EquippedSkillID))
	{
		return;
	}
	
	const float ManaCost = GetSkillManaCost(EquippedSkillID);
	
	if (!CanSpendMana(ManaCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Enough Mana"));
		return;
	}

	if (DoesSkillNeedCasting(EquippedSkillID))
	{
		StartSkillCast(
			SkillSlot,
			EquippedSkillID,
			TargetLocation
		);

		return;
	}
	
	const bool bSkillSucceeded =
		ExecuteSkillByID(EquippedSkillID, TargetLocation);
	
	if (bSkillSucceeded)
	{
		CommitSkillCostAndCooldown(EquippedSkillID);
	}
}


bool ALPlayerCharacter::UseMeteorSkill(const FVector& TargetLocation)
{
	if (!GetSkillDataAsset(ELPlayerSkillID::Meteor))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("UseMeteorSkill Failed: DA_Meteor is not configured in SkillDatabase")
		);

		return false;
	}

	UClass* SkillActorClass = GetSkillActorClass(ELPlayerSkillID::Meteor);

	if (!SkillActorClass || !SkillActorClass->IsChildOf(ALMeteorActor::StaticClass()))
	{
		return false;
	}

	FVector AttackDirection;
	FRotator AttackRotation;

	if (!PrepareActionDirection(TargetLocation, AttackDirection, AttackRotation))
	{
		return false;
	}

	FVector ImpactLocation = TargetLocation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ALMeteorActor* MeteorActor = GetWorld()->SpawnActor<ALMeteorActor>(
		SkillActorClass,
		ImpactLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!MeteorActor)
	{
		return false;
	}

	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::Meteor);

	MeteorActor->InitializeMeteor(
		ImpactLocation,
		CombatTuning.Damage,
		CombatTuning.Radius,
		CombatTuning.SpawnHeightOffset,
		CombatTuning.Duration,
		GetSkillWarningNiagara(ELPlayerSkillID::Meteor),
		GetSkillImpactNiagara(ELPlayerSkillID::Meteor)
	);

	StartSkillLock(ELPlayerSkillID::Meteor);

	return true;
}

bool ALPlayerCharacter::UseIceLanceSkill(const FVector& TargetLocation)
{
	if (!GetSkillDataAsset(ELPlayerSkillID::IceLance))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("UseIceLanceSkill Failed: DA_IceLance is not configured in SkillDatabase")
		);

		return false;
	}

	UClass* SkillActorClass = GetSkillActorClass(ELPlayerSkillID::IceLance);

	if (!SkillActorClass || !SkillActorClass->IsChildOf(ALIceLanceActor::StaticClass()))
	{
		return false;
	}

	FVector SkillDirection;
	FRotator SkillRotation;

	if (!PrepareActionDirection(TargetLocation, SkillDirection, SkillRotation))
	{
		return false;
	}

	const FVector RightDirection = FVector::CrossProduct(
		FVector::UpVector,
		SkillDirection
	).GetSafeNormal();
	
	SetCurrentActionState(ELPlayerActionState::Skill);

	const FLPlayerIceLanceTuning IceLanceTuning =
		GetSkillIceLanceTuning();
	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::IceLance);
	
	const int32 LanceCount = FMath::Max(1, IceLanceTuning.Count);
	const float CenterIndex = (LanceCount - 1) * 0.5f;
	
	for (int32 i = 0; i < LanceCount; ++i)
	{
		const float SideIndex = i - CenterIndex;
		
		FVector StartLocation =
			GetActorLocation()
			- SkillDirection * IceLanceTuning.ReadyBackOffset
			+ RightDirection * (SideIndex * IceLanceTuning.ReadySideSpacing);

		const float HeightOffset =
			IceLanceTuning.ReadyHeight
			- FMath::Abs(SideIndex) * IceLanceTuning.ReadyHeightFalloff;

		StartLocation.Z += HeightOffset;
		
		FVector EndLocation = TargetLocation;
		EndLocation.Z += IceLanceTuning.EndHeightOffset;
		
		FVector ControlLocation =
			(StartLocation + EndLocation) * 0.5f;

		ControlLocation += RightDirection * (SideIndex * IceLanceTuning.CurveSideOffset);
		ControlLocation.Z += IceLanceTuning.CurveHeightOffset;

		const float FireDelay =
			IceLanceTuning.ReadyDuration
			+ FMath::Abs(SideIndex) * IceLanceTuning.FireInterval;
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		ALIceLanceActor* IceLance =
			GetWorld()->SpawnActor<ALIceLanceActor>(
				SkillActorClass,
				StartLocation,
				SkillRotation,
				SpawnParams
			);

		if (IceLance)
		{
			IceLance->InitializeBezierPath(
				StartLocation,
				ControlLocation,
				EndLocation,
				IceLanceTuning.TravelDuration,
				FireDelay,
				CombatTuning.Damage,
				CombatTuning.Radius,
				GetSkillImpactNiagara(ELPlayerSkillID::IceLance)
			);
		}
	}
	
	if (UNiagaraSystem* IceLanceNiagara = GetSkillMainNiagara(ELPlayerSkillID::IceLance))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(	
			GetWorld(),
			IceLanceNiagara,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	StartSkillLock(ELPlayerSkillID::IceLance);

	return true;
}

bool ALPlayerCharacter::UseThunderSkill(const FVector& TargetLocation)
{
	if (!GetSkillDataAsset(ELPlayerSkillID::Thunder))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("UseThunderSkill Failed: DA_Thunder is not configured in SkillDatabase")
		);

		return false;
	}

	UClass* SkillActorClass = GetSkillActorClass(ELPlayerSkillID::Thunder);

	if (!SkillActorClass || !SkillActorClass->IsChildOf(ALThunderActor::StaticClass()))
	{
		return false;
	}

	FVector AttackDirection;
	FRotator AttackRotation;

	if (!PrepareActionDirection(TargetLocation, AttackDirection, AttackRotation))
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ALThunderActor* ThunderStormActor =
		GetWorld()->SpawnActor<ALThunderActor>(
			SkillActorClass,
			TargetLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);

	if (!ThunderStormActor)
	{
		return false;
	}

	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::Thunder);

	ThunderStormActor->InitializeThunderStorm(
		TargetLocation,
		CombatTuning.Damage,
		CombatTuning.Radius,
		CombatTuning.SpawnRadius,
		CombatTuning.Count,
		CombatTuning.TickInterval,
		CombatTuning.SpawnHeightOffset,
		GetSkillMainNiagara(ELPlayerSkillID::Thunder)
	);

	StartSkillLock(ELPlayerSkillID::Thunder);

	return true;
}

bool ALPlayerCharacter::UseWindSkill(const FVector& TargetLocation)
{
	if (!GetSkillDataAsset(ELPlayerSkillID::Wind))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("UseWindSkill Failed: DA_Wind is not configured in SkillDatabase")
		);

		return false;
	}

	FVector AttackDirection;
	FRotator AttackRotation;

	if (!PrepareActionDirection(TargetLocation, AttackDirection, AttackRotation))
	{
		return false;
	}

	SetCurrentActionState(ELPlayerActionState::Skill);

	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::Wind);

	FVector SpawnLocation =
		GetActorLocation()
		+ AttackDirection * CombatTuning.ForwardOffset;

	SpawnLocation.Z += CombatTuning.HeightOffset;

	if (UNiagaraSystem* WindNiagara = GetSkillMainNiagara(ELPlayerSkillID::Wind))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			WindNiagara,
			SpawnLocation,
			AttackRotation
		);
	}

	ApplyWindDamage(AttackDirection);
	PlaySkillImpactSound(ELPlayerSkillID::Wind, SpawnLocation);

	StartSkillLock(ELPlayerSkillID::Wind);

	return true;
}

bool ALPlayerCharacter::UseFrostFieldSkill(const FVector& TargetLocation)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return false;
	}

	if (!GetSkillDataAsset(ELPlayerSkillID::FrostField))
	{
		UE_LOG(LogTemp, Warning, TEXT("FrostField failed: DA_FrostField is not set."));
		return false;
	}

	FVector SkillDirection = TargetLocation - GetActorLocation();
	SkillDirection.Z = 0.0f;

	if (!SkillDirection.IsNearlyZero())
	{
		SkillDirection.Normalize();
		const FRotator SkillRotation = SkillDirection.Rotation();
		SetActorRotation(FRotator(0.0f, SkillRotation.Yaw, 0.0f));
	}

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	const FLPlayerSkillCombatTuning CombatTuning =
		GetSkillCombatTuning(ELPlayerSkillID::FrostField);

	FVector SpawnLocation = TargetLocation;
	SpawnLocation.Z += CombatTuning.SpawnHeightOffset;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	TSubclassOf<ALGroundAreaSkillActor> AreaActorClass =
		ALGroundAreaSkillActor::StaticClass();
	UClass* DataActorClass = GetSkillActorClass(ELPlayerSkillID::FrostField);

	if (DataActorClass)
	{
		if (DataActorClass->IsChildOf(ALGroundAreaSkillActor::StaticClass()))
		{
			AreaActorClass = DataActorClass;
		}
		else
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("FrostField SkillActorClass must inherit from ALGroundAreaSkillActor: %s"),
				*DataActorClass->GetName()
			);
			return false;
		}
	}

	ALGroundAreaSkillActor* FrostFieldActor =
		World->SpawnActor<ALGroundAreaSkillActor>(
			AreaActorClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);

	if (!FrostFieldActor)
	{
		return false;
	}

	FrostFieldActor->InitializeGroundAreaSkill(
		ELPlayerSkillID::FrostField,
		SpawnLocation,
		CombatTuning.Damage,
		CombatTuning.Radius,
		CombatTuning.Duration,
		CombatTuning.TickInterval,
		GetSkillMainNiagara(ELPlayerSkillID::FrostField)
	);

	StartSkillLock(ELPlayerSkillID::FrostField);

	return true;
}

bool ALPlayerCharacter::UseMeteorRainSkill(const FVector& TargetLocation)
{
	// 아직 MeteorRain 구현 전이므로 실패 처리.
	// false를 반환해야 V를 눌러도 쿨타임이 돌지 않는다.
	return false;
}

void ALPlayerCharacter::CancelCurrentAction()
{
	const ELPlayerActionState ActionState = GetCurrentActionState();

	if (ActionState == ELPlayerActionState::Casting || bIsCasting)
	{
		CancelSkillCast();
	}

	if (ActionState == ELPlayerActionState::BasicAttack)
	{
		GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);

		if (const ULPlayerSkillDataAsset* BasicAttackDataAsset =
			GetSkillDataAsset(ELPlayerSkillID::BasicAttack))
		{
			StopAnimMontage(BasicAttackDataAsset->SkillMontage);
		}

		bBasicAttackHitTriggered = false;
	}

	if (ActionState == ELPlayerActionState::Skill)
	{
		GetWorldTimerManager().ClearTimer(SkillTimerHandle);
		StopActiveSkillMontage();
	}
	
	Super::CancelCurrentAction();
}

void ALPlayerCharacter::SaveEquippedSkillSlots()
{
	ULPlayerSaveGame* SaveGameInstance = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(
		PlayerSaveSlotName,
		PlayerSaveUserIndex
	))
	{
		SaveGameInstance = Cast<ULPlayerSaveGame>(
			UGameplayStatics::LoadGameFromSlot(
				PlayerSaveSlotName,
				PlayerSaveUserIndex
			)
		);
	}

	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<ULPlayerSaveGame>(
			UGameplayStatics::CreateSaveGameObject(
				ULPlayerSaveGame::StaticClass()
			)
		);
	}

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveEquippedSkillSlots Failed: SaveGameInstance is null"));
		return;
	}

	SaveGameInstance->SavedSkillSlots.Empty();

	const TArray<ELPlayerSkillSlot> SkillSlots =
	{
		ELPlayerSkillSlot::Q,
		ELPlayerSkillSlot::W,
		ELPlayerSkillSlot::E,
		ELPlayerSkillSlot::R,
		ELPlayerSkillSlot::A,
		ELPlayerSkillSlot::S,
		ELPlayerSkillSlot::D,
		ELPlayerSkillSlot::F,
		ELPlayerSkillSlot::V
	};

	for (const ELPlayerSkillSlot SkillSlot : SkillSlots)
	{
		FLPlayerSkillSlotSaveData SaveData;
		SaveData.SkillSlot = SkillSlot;
		SaveData.SkillID = GetEquippedSkillID(SkillSlot);

		SaveGameInstance->SavedSkillSlots.Add(SaveData);
	}

	const bool bSaved = UGameplayStatics::SaveGameToSlot(
		SaveGameInstance,
		PlayerSaveSlotName,
		PlayerSaveUserIndex
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("SaveEquippedSkillSlots / Success=%d / Count=%d"),
		bSaved,
		SaveGameInstance->SavedSkillSlots.Num()
	);
}

void ALPlayerCharacter::LoadEquippedSkillSlots()
{
	if (!UGameplayStatics::DoesSaveGameExist(
		PlayerSaveSlotName,
		PlayerSaveUserIndex
	))
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadEquippedSkillSlots: Save file does not exist. Use default slots."));
		return;
	}

	ULPlayerSaveGame* SaveGameInstance = Cast<ULPlayerSaveGame>(
		UGameplayStatics::LoadGameFromSlot(
			PlayerSaveSlotName,
			PlayerSaveUserIndex
		)
	);

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadEquippedSkillSlots Failed: SaveGameInstance is null"));
		return;
	}

	for (const FLPlayerSkillSlotSaveData& SaveData : SaveGameInstance->SavedSkillSlots)
	{
		SetEquippedSkillID(
			SaveData.SkillSlot,
			SaveData.SkillID
		);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Load Skill Slot / Slot=%s / Skill=%s"),
			*UEnum::GetValueAsString(SaveData.SkillSlot),
			*UEnum::GetValueAsString(SaveData.SkillID)
		);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("LoadEquippedSkillSlots Complete / Count=%d"),
		SaveGameInstance->SavedSkillSlots.Num()
	);
}
