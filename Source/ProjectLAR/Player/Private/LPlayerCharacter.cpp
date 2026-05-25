// Fill out your copyright notice in the Description page of Project Settings.

#include "LPlayerCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
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
// Skill Cooldown

bool ALPlayerCharacter::CanUseSkillSlot(ELPlayerSkillSlot SkillSlot) const
{
	return CanUseSkill()
		&& !IsSkillOnCooldown(SkillSlot);
}

bool ALPlayerCharacter::IsSkillOnCooldown(ELPlayerSkillSlot SkillSlot) const
{
	if (!GetWorld())
	{
		return false;
	}

	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		return GetWorldTimerManager().IsTimerActive(QSkillCooldownTimerHandle);

	case ELPlayerSkillSlot::W:
		return GetWorldTimerManager().IsTimerActive(WSkillCooldownTimerHandle);

	case ELPlayerSkillSlot::E:
		return GetWorldTimerManager().IsTimerActive(ESkillCooldownTimerHandle);

	case ELPlayerSkillSlot::R:
		return GetWorldTimerManager().IsTimerActive(RSkillCooldownTimerHandle);

	case ELPlayerSkillSlot::V:
		return GetWorldTimerManager().IsTimerActive(VSkillCooldownTimerHandle);

	default:
		return false;
	}
}

float ALPlayerCharacter::GetSkillCooldownRemaining(ELPlayerSkillSlot SkillSlot) const
{
	if (!GetWorld())
	{
		return 0.0f;
	}

	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		return GetWorldTimerManager().IsTimerActive(QSkillCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(QSkillCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillSlot::W:
		return GetWorldTimerManager().IsTimerActive(WSkillCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(WSkillCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillSlot::E:
		return GetWorldTimerManager().IsTimerActive(ESkillCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(ESkillCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillSlot::R:
		return GetWorldTimerManager().IsTimerActive(RSkillCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(RSkillCooldownTimerHandle))
			: 0.0f;

	case ELPlayerSkillSlot::V:
		return GetWorldTimerManager().IsTimerActive(VSkillCooldownTimerHandle)
			? FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(VSkillCooldownTimerHandle))
			: 0.0f;

	default:
		return 0.0f;
	}
}

float ALPlayerCharacter::GetSkillCooldownRatio(ELPlayerSkillSlot SkillSlot) const
{
	const float CooldownDuration = GetSkillCooldownDuration(SkillSlot);

	if (CooldownDuration <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(
		GetSkillCooldownRemaining(SkillSlot) / CooldownDuration,
		0.0f,
		1.0f
	);
}

float ALPlayerCharacter::GetSkillCooldownDuration(ELPlayerSkillSlot SkillSlot) const
{
	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		return QSkillCooldown;

	case ELPlayerSkillSlot::W:
		return WSkillCooldown;

	case ELPlayerSkillSlot::E:
		return ESkillCooldown;

	case ELPlayerSkillSlot::R:
		return RSkillCooldown;

	case ELPlayerSkillSlot::V:
		return VSkillCooldown;

	default:
		return 0.0f;
	}
}

void ALPlayerCharacter::StartSkillCooldown(ELPlayerSkillSlot SkillSlot)
{
	if (!GetWorld())
	{
		return;
	}

	const float CooldownDuration = GetSkillCooldownDuration(SkillSlot);

	if (CooldownDuration <= 0.0f)
	{
		return;
	}

	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		GetWorldTimerManager().ClearTimer(QSkillCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			QSkillCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetQSkillCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillSlot::W:
		GetWorldTimerManager().ClearTimer(WSkillCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			WSkillCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetWSkillCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillSlot::E:
		GetWorldTimerManager().ClearTimer(ESkillCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			ESkillCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetESkillCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillSlot::R:
		GetWorldTimerManager().ClearTimer(RSkillCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			RSkillCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetRSkillCooldown,
			CooldownDuration,
			false
		);
		break;

	case ELPlayerSkillSlot::V:
		GetWorldTimerManager().ClearTimer(VSkillCooldownTimerHandle);
		GetWorldTimerManager().SetTimer(
			VSkillCooldownTimerHandle,
			this,
			&ALPlayerCharacter::ResetVSkillCooldown,
			CooldownDuration,
			false
		);
		break;

	default:
		break;
	}
}

void ALPlayerCharacter::ResetQSkillCooldown()
{
}

void ALPlayerCharacter::ResetWSkillCooldown()
{
}

void ALPlayerCharacter::ResetESkillCooldown()
{
}

void ALPlayerCharacter::ResetRSkillCooldown()
{
}

void ALPlayerCharacter::ResetVSkillCooldown()
{
}

// =======================================================================================
// Skill Use

void ALPlayerCharacter::UseSkill(
	ELPlayerSkillSlot SkillSlot,
	const FVector& TargetLocation)
{
	if (!CanUseSkillSlot(SkillSlot))
	{
		return;
	}

	bool bSkillSucceeded = false;

	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		bSkillSucceeded = UseQSkill(TargetLocation);
		break;
		
	case ELPlayerSkillSlot::W:
		bSkillSucceeded = UseWSkill(TargetLocation);
		break;
		
	case ELPlayerSkillSlot::E:
		bSkillSucceeded = UseESkill(TargetLocation);
		break;
		
	case ELPlayerSkillSlot::R:
		bSkillSucceeded = UseRSkill(TargetLocation);
		break;
		
	case ELPlayerSkillSlot::V:
		bSkillSucceeded = UseVSkill(TargetLocation);
		break;

	default:
		break;
	}

	if (bSkillSucceeded)
	{
		StartSkillCooldown(SkillSlot);
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
		QSkillLockDuration,
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
		WSkillLockDuration,
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
		ESkillLockDuration,
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
		+ AttackDirection * RWindForwardOffset;

	SpawnLocation.Z += RWindHeightOffset;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		RWindNiagara,
		SpawnLocation,
		AttackRotation
	);

	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		RSkillLockDuration,
		false
	);

	return true;
}

bool ALPlayerCharacter::UseVSkill(const FVector& TargetLocation)
{
	// 아직 V 스킬 구현 전이므로 실패 처리.
	// false를 반환해야 V를 눌러도 쿨타임이 돌지 않는다.
	return false;
}

void ALPlayerCharacter::CancelCurrentAction()
{
	GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);
	GetWorldTimerManager().ClearTimer(SkillTimerHandle);
	
	Super::CancelCurrentAction();
}