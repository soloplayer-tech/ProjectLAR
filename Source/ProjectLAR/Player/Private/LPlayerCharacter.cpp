// Fill out your copyright notice in the Description page of Project Settings.

#include "LPlayerCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "ProjectLAR/Skill/Public/LIceLanceActor.h"

ALPlayerCharacter::ALPlayerCharacter()
{
	bBlink = true;
}

void ALPlayerCharacter::Dash(const FVector& DashDirection)
{
	if (!CanDash())
	{
		return;
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

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetHiddenInGame(false, true);
	}
}

void ALPlayerCharacter::BasicAttack(const FVector& TargetLocation)
{
	FVector AttackDirection = TargetLocation - GetActorLocation();
	AttackDirection.Z = 0.0f;

	if (AttackDirection.IsNearlyZero())
	{
		return;
	}

	AttackDirection.Normalize();
	
	const FRotator AttackRotation = AttackDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			AttackRotation.Yaw,
			0.0f
		)
	);
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	SetCurrentActionState(ELPlayerActionState::BasicAttack);
	
	FVector SpawnLocation =
		GetActorLocation()
		+ AttackDirection * BasicAttackForwardOffset;

	SpawnLocation.Z += BasicAttackHeightOffset;
	
	if (BasicAttackNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BasicAttackNiagara,
			SpawnLocation,
			AttackRotation
		);
	}
	
	ApplyBasicAttackDamage(AttackDirection);
	
	GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);

	GetWorldTimerManager().SetTimer(
		BasicAttackTimerHandle,
		this,
		&ALPlayerCharacter::EndBasicAttack,
		BasicAttackDuration,
		false
	);
}

void ALPlayerCharacter::EndBasicAttack()
{
	SetCurrentActionState(ELPlayerActionState::Idle);
}

void ALPlayerCharacter::EndSkill()
{
	SetCurrentActionState(ELPlayerActionState::Idle);
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

	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return GetWorldTimerManager().IsTimerActive(MeteorCooldownTimerHandle);

	case ELPlayerSkillID::IceLance:
		return GetWorldTimerManager().IsTimerActive(IceLanceCooldownTimerHandle);

	case ELPlayerSkillID::Thunder:
		return GetWorldTimerManager().IsTimerActive(ThunderCooldownTimerHandle);

	case ELPlayerSkillID::Wind:
		return GetWorldTimerManager().IsTimerActive(WindCooldownTimerHandle);

	case ELPlayerSkillID::MeteorRain:
		return GetWorldTimerManager().IsTimerActive(MeteorRainCooldownTimerHandle);

	case ELPlayerSkillID::None:
	default:
		return false;
	}
}

float ALPlayerCharacter::GetSkillIDCooldownRemaining(ELPlayerSkillID SkillID) const
{
	if (!GetWorld())
	{
		return 0.0f;
	}

	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return GetWorldTimerManager().IsTimerActive(MeteorCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(MeteorCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillID::IceLance:
		return GetWorldTimerManager().IsTimerActive(IceLanceCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(IceLanceCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillID::Thunder:
		return GetWorldTimerManager().IsTimerActive(ThunderCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(ThunderCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillID::Wind:
		return GetWorldTimerManager().IsTimerActive(WindCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(WindCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillID::MeteorRain:
		return GetWorldTimerManager().IsTimerActive(MeteorRainCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(MeteorRainCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillID::None:
	default:
		return 0.0f;
	}
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
	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return MeteorCooldown;

	case ELPlayerSkillID::IceLance:
		return IceLanceCooldown;

	case ELPlayerSkillID::Thunder:
		return ThunderCooldown;

	case ELPlayerSkillID::Wind:
		return WindCooldown;

	case ELPlayerSkillID::MeteorRain:
		return MeteorRainCooldown;

	case ELPlayerSkillID::None:
	default:
		return 0.0f;
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

	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		GetWorldTimerManager().ClearTimer(MeteorCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			MeteorCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetMeteorCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillID::IceLance:
		GetWorldTimerManager().ClearTimer(IceLanceCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			IceLanceCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetIceLanceCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillID::Thunder:
		GetWorldTimerManager().ClearTimer(ThunderCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			ThunderCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetThunderCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillID::Wind:
		GetWorldTimerManager().ClearTimer(WindCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			WindCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetWindCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillID::MeteorRain:
		GetWorldTimerManager().ClearTimer(MeteorRainCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			MeteorRainCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetMeteorRainCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillID::None:
	default:
		break;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Skill Cooldown Start: %s / %.1f"),
		*UEnum::GetValueAsString(SkillID),
		CooldownDuration
	);
}

void ALPlayerCharacter::ResetMeteorCooldown()
{
}

void ALPlayerCharacter::ResetIceLanceCooldown()
{
}

void ALPlayerCharacter::ResetThunderCooldown()
{
}

void ALPlayerCharacter::ResetWindCooldown()
{
}

void ALPlayerCharacter::ResetMeteorRainCooldown()
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
		
	default:
		break;
	}
}

bool ALPlayerCharacter::ExecuteSkillByID(
	ELPlayerSkillID SkillID,
	const FVector& TargetLocation)
{
	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return UseQSkill(TargetLocation);
		
	case ELPlayerSkillID::IceLance:
		return UseWSkill(TargetLocation);
		
	case ELPlayerSkillID::Thunder:
		return UseESkill(TargetLocation);
		
	case ELPlayerSkillID::Wind:
		return UseRSkill(TargetLocation);
		
	case ELPlayerSkillID::MeteorRain:
		return UseVSkill(TargetLocation);

	case ELPlayerSkillID::None:
	default:
		return false;
	}
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
	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
	case ELPlayerSkillID::Thunder:
		return true;

	default:
		return false;
	}
}

float ALPlayerCharacter::GetSkillCastDuration(ELPlayerSkillID SkillID) const
{
	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return MeteorCastDuration;

	case ELPlayerSkillID::Thunder:
		return ThunderCastDuration;

	default:
		return 0.0f;
	}
}

// ======================================================================================
// 스킬 캐스팅 시작할 때 나이아가라 시작 / 스킬 발동 후는 아님

void ALPlayerCharacter::SpawnCastStartEffect(
	ELPlayerSkillID SkillID,
	const FVector& TargetLocation
)
{
	UNiagaraSystem* CastStartEffect = nullptr;
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		// 메테오 캐스팅 이펙트는 플레이어 발밑
		CastStartEffect = MeteorCastStartEffect;
		SpawnLocation = GetActorLocation();
		SpawnLocation.Z += MeteorCastEffectHeightOffset;
		SpawnRotation = GetActorRotation();
		break;

	case ELPlayerSkillID::Thunder:
		// 썬더 캐스팅 이펙트는 마우스 위치
		CastStartEffect = ThunderCastStartEffect;
		SpawnLocation = TargetLocation;
		SpawnLocation.Z += ThunderCastEffectHeightOffset;
		SpawnRotation = FRotator::ZeroRotator;
		break;

	default:
		break;
	}

	if (!CastStartEffect)
	{
		return;
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
		const bool bSkillSucceeded =
			ExecuteSkillByID(SkillID, TargetLocation);

		if (bSkillSucceeded)
		{
			StartSkillCooldown(SkillID);
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

	const bool bSkillSucceeded =
		ExecuteSkillByID(FinishedSkillID, FinishedTargetLocation);

	if (bSkillSucceeded)
	{
		StartSkillCooldown(FinishedSkillID);
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

	FVector DamageCenter =
		GetActorLocation()
		+ NormalizedDirection * BasicAttackDamageCenterOffset;

	DamageCenter.Z += BasicAttackDamageHeightOffset;

	if (bDrawBasicAttackDamageDebug)
	{
		DrawDebugBox(
			World,
			DamageCenter,
			BasicAttackDamageBoxHalfExtent,
			DamageRotation.Quaternion(),
			FColor::Yellow,
			false,
			1.0f
		);
	}

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
		FCollisionShape::MakeBox(BasicAttackDamageBoxHalfExtent),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	TArray<AActor*> DamagedActors;

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

		ApplySkillDamageToActor(
			HitActor,
			BasicAttackDamage,
			ELPlayerSkillID::BasicAttack
		);
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

	FVector DamageCenter =
		GetActorLocation()
		+ NormalizedDirection * WindDamageCenterOffset;

	DamageCenter.Z += WindDamageHeightOffset;

	if (bDrawWindDamageDebug)
	{
		DrawDebugBox(
			World,
			DamageCenter,
			WindDamageBoxHalfExtent,
			DamageRotation.Quaternion(),
			FColor::Red,
			false,
			1.0f
		);
	}

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
		FCollisionShape::MakeBox(WindDamageBoxHalfExtent),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	TArray<AActor*> DamagedActors;

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

		ApplySkillDamageToActor(
			HitActor,
			WindDamage,
			ELPlayerSkillID::Wind
		);
	}
}

void ALPlayerCharacter::ApplySkillDamageToActor(
	AActor* TargetActor,
	float Damage,
	ELPlayerSkillID SkillID
)
{
	if (!TargetActor)
	{
		return;
	}

	if (TargetActor == this)
	{
		return;
	}

	if (!TargetActor->GetClass()->ImplementsInterface(ULDamageable::StaticClass()))
	{
		return;
	}

	ILDamageable::Execute_ReceiveSkillDamage(
		TargetActor,
		Damage,
		this,
		SkillID
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Skill Damage Applied: %s / Skill: %s / Damage: %.1f"),
		*TargetActor->GetName(),
		*UEnum::GetValueAsString(SkillID),
		Damage
	);
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
		StartSkillCooldown(EquippedSkillID);
	}
}

bool ALPlayerCharacter::UseQSkill(const FVector& TargetLocation)
{
	if (!MeteorActorClass)
	{
		return false;
	}

	FVector AttackDirection = TargetLocation - GetActorLocation();
	AttackDirection.Z = 0.0f;

	if (AttackDirection.IsNearlyZero())
	{
		return false;
	}

	AttackDirection.Normalize();

	const FRotator AttackRotation = AttackDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			AttackRotation.Yaw,
			0.0f
		)
	);

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	FVector ImpactLocation = TargetLocation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ALMeteorActor* MeteorActor = GetWorld()->SpawnActor<ALMeteorActor>(
		MeteorActorClass,
		ImpactLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!MeteorActor)
	{
		return false;
	}

	MeteorActor->InitializeMeteor(ImpactLocation);

	SetCurrentActionState(ELPlayerActionState::Skill);

	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		MeteorSkillLockDuration,
		false
	);

	return true;
}

bool ALPlayerCharacter::UseWSkill(const FVector& TargetLocation)
{
	if (!IceLanceClass)
	{
		return false;
	}
	
	FVector SkillDirection = TargetLocation - GetActorLocation();
	SkillDirection.Z = 0.0f;

	if (SkillDirection.IsNearlyZero())
	{
		return false;
	}

	SkillDirection.Normalize();

	const FVector RightDirection = FVector::CrossProduct(
		FVector::UpVector,
		SkillDirection
	).GetSafeNormal();

	const FRotator SkillRotation = SkillDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			SkillRotation.Yaw,
			0.0f
		)
	);
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	SetCurrentActionState(ELPlayerActionState::Skill);
	
	const float CenterIndex = (IceLanceCount - 1) * 0.5f;
	
	for (int32 i = 0; i < IceLanceCount; ++i)
	{
		const float SideIndex = i - CenterIndex;
		
		FVector StartLocation =
			GetActorLocation()
			- SkillDirection * IceLanceReadyBackOffset
			+ RightDirection * (SideIndex * IceLanceReadySideSpacing);

		const float HeightOffset =
			IceLanceReadyHeight
			- FMath::Abs(SideIndex) * IceLanceReadyHeightFalloff;

		StartLocation.Z += HeightOffset;
		
		FVector EndLocation = TargetLocation;
		EndLocation.Z += IceLanceEndHeightOffset;
		
		FVector ControlLocation =
			(StartLocation + EndLocation) * 0.5f;

		ControlLocation += RightDirection * (SideIndex * IceLanceCurveSideOffset);
		ControlLocation.Z += IceLanceCurveHeightOffset;

		const float FireDelay =
			IceLanceReadyDuration
			+ FMath::Abs(SideIndex) * IceLanceFireInterval;
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		ALIceLanceActor* IceLance =
			GetWorld()->SpawnActor<ALIceLanceActor>(
				IceLanceClass,
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
				IceLanceTravelDuration,
				FireDelay
			);
		}
	}
	
	if (WIceLanceNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(	
			GetWorld(),
			WIceLanceNiagara,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		IceLanceSkillLockDuration,
		false
	);

	return true;
}

bool ALPlayerCharacter::UseESkill(const FVector& TargetLocation)
{
	if (!ThunderStormActorClass)
	{
		return false;
	}

	FVector AttackDirection = TargetLocation - GetActorLocation();
	AttackDirection.Z = 0.0f;

	if (AttackDirection.IsNearlyZero())
	{
		return false;
	}

	AttackDirection.Normalize();

	const FRotator AttackRotation = AttackDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			AttackRotation.Yaw,
			0.0f
		)
	);

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ALThunderActor* ThunderStormActor =
		GetWorld()->SpawnActor<ALThunderActor>(
			ThunderStormActorClass,
			TargetLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);

	if (!ThunderStormActor)
	{
		return false;
	}

	ThunderStormActor->InitializeThunderStorm(TargetLocation);

	SetCurrentActionState(ELPlayerActionState::Skill);

	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		ThunderSkillLockDuration,
		false
	);

	return true;
}

bool ALPlayerCharacter::UseRSkill(const FVector& TargetLocation)
{
	if (!RWindNiagara)
	{
		return false;
	}

	FVector AttackDirection = TargetLocation - GetActorLocation();
	AttackDirection.Z = 0.0f;

	if (AttackDirection.IsNearlyZero())
	{
		return false;
	}

	AttackDirection.Normalize();

	const FRotator AttackRotation = AttackDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			AttackRotation.Yaw,
			0.0f
		)
	);

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	SetCurrentActionState(ELPlayerActionState::Skill);

	FVector SpawnLocation =
		GetActorLocation()
		+ AttackDirection * WindForwardOffset;

	SpawnLocation.Z += WindHeightOffset;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		RWindNiagara,
		SpawnLocation,
		AttackRotation
	);
	
	ApplyWindDamage(AttackDirection);
	
	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		WindSkillLockDuration,
		false
	);

	return true;
}

bool ALPlayerCharacter::UseVSkill(const FVector& TargetLocation)
{
	// 아직 MeteorRain 구현 전이므로 실패 처리.
	// false를 반환해야 V를 눌러도 쿨타임이 돌지 않는다.
	return false;
}

void ALPlayerCharacter::CancelCurrentAction()
{
	CancelSkillCast();

	GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);
	GetWorldTimerManager().ClearTimer(SkillTimerHandle);
	
	Super::CancelCurrentAction();
}