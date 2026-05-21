// Fill out your copyright notice in the Description page of Project Settings.

#include "LPlayerCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
//#include "NiagaraSystem.h"
#include "ProjectLAR/Skill/Public/LIceLanceActor.h"
#include "TimerManager.h"

ALPlayerCharacter::ALPlayerCharacter()
{
	bBlink = true;
}

void ALPlayerCharacter::Dash(const FVector& DashDirection)
{
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

		GetWorldTimerManager().SetTimer(
			BlinkTimerHandle,
			this,
			&ALPlayerCharacter::EndBlink,
			BlinkDuration,
			false
		);
	}

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	Super::Dash(DashDirection);
}

void ALPlayerCharacter::EndSkill()
{
	SetCurrentActionState(ELPlayerActionState::Idle);
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
	// 공격 방향 계산
	FVector AttackDirection = TargetLocation - GetActorLocation();
	AttackDirection.Z = 0.0f;

	if (AttackDirection.IsNearlyZero())
	{
		return;
	}

	AttackDirection.Normalize();
	
	// 마우스 방향으로 캐릭터 회전
	const FRotator AttackRotation = AttackDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			AttackRotation.Yaw,
			0.0f
		)
	);
	
	// 현재 움직임 즉시 정지상태로 변경
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	// 상태를 BasicAttack으로 바꾸기
	SetCurrentActionState(ELPlayerActionState::BasicAttack);
	
	// 나이아가라 생성 위치
	FVector SpawnLocation =
		GetActorLocation() +
		AttackDirection * BasicAttackForwardOffset;

	SpawnLocation.Z += BasicAttackHeightOffset;
	
	// 나이아가라 공격 이팩트 생성
	if (BasicAttackNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BasicAttackNiagara,
			SpawnLocation,
			AttackRotation
		);
	}
	
	// 공격 종료 타이머
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

void ALPlayerCharacter::UseQSkill(const FVector& TargetLocation)
{
	// 캐릭터가 마우스 위치를 바라보게 하기
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
	
	// 현재 움직임 즉시 정지
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	// 상태를 Skill로 변경
	SetCurrentActionState(ELPlayerActionState::Skill);
	
	// 메테오 생성 위치 계산
	FVector SpawnLocation = TargetLocation;

	// 메테오가 위에서 떨어지는 Niagara라면 높은 위치에서 생성
	SpawnLocation.Z += QMeteorSpawnHeight;
	
	// 메테오 Niagara 생성
	if (QMeteorNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			QMeteorNiagara,
			SpawnLocation,
			FRotator::ZeroRotator
		);
	}
	
	// 스킬 종료 타이머
	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		QMeteorDuration,
		false
	);
}

void ALPlayerCharacter::UseWSkill(const FVector& TargetLocation)
{
	if (!IceLanceClass)
	{
		return;
	}
	
	// 공격 방향 계산
	FVector SkillDirection = TargetLocation - GetActorLocation();
	SkillDirection.Z = 0.0f;

	if (SkillDirection.IsNearlyZero())
	{
		return;
	}

	SkillDirection.Normalize();

	// 캐릭터 기준 오른쪽 방향
	const FVector RightDirection = FVector::CrossProduct(
		FVector::UpVector,
		SkillDirection
	).GetSafeNormal();

	// 캐릭터가 마우스 방향 바라보기
	const FRotator SkillRotation = SkillDirection.Rotation();

	SetActorRotation(
		FRotator(
			0.0f,
			SkillRotation.Yaw,
			0.0f
		)
	);
	
	// 이동 정지
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	// 상태 Skill
	SetCurrentActionState(ELPlayerActionState::Skill);
	
	// 얼음창 5개 생성
	const float CenterIndex = (IceLanceCount - 1) * 0.5f;
	
	for (int32 i = 0; i < IceLanceCount; ++i)
	{
		const float SideIndex = i - CenterIndex;
		
		// 얼음창 준비 위치
		FVector StartLocation =
			GetActorLocation()
			- SkillDirection * IceLanceReadyBackOffset
			+ RightDirection * (SideIndex * IceLanceReadySideSpacing);

		const float HeightOffset =
			IceLanceReadyHeight
			- FMath::Abs(SideIndex) * IceLanceReadyHeightFalloff;

		StartLocation.Z += HeightOffset;
		
		// 도착점
		FVector EndLocation = TargetLocation;
		EndLocation.Z += IceLanceEndHeightOffset;
		
		// 베지어 조절점
		FVector ControlLocation =
			(StartLocation + EndLocation) * 0.5f;

		ControlLocation += RightDirection * (SideIndex * IceLanceCurveSideOffset);
		ControlLocation.Z += IceLanceCurveHeightOffset;
		// 발사 딜레이
		// 가운데  안쪽  바깥쪽 순서
		const float FireDelay =
			IceLanceReadyDuration
			+ FMath::Abs(SideIndex) * IceLanceFireInterval;
		
		// 얼음창 액터 소환
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
	// 스킬 행동 잠금 종료 타이머
	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		WSkillLockDuration,
		false
	);
}

void ALPlayerCharacter::UseESkill(const FVector& TargetLocation)
{
	// 캐릭터가 마우스 위치를 바라보게 하기
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
	
	// 현재 움직임 즉시 정지
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	// 상태를 Skill로 변경
	SetCurrentActionState(ELPlayerActionState::Skill);
	
	// 번개 생성 위치 계산
	FVector SpawnLocation = TargetLocation;
	
	// 번개 Niagara 생성
	if (EThunderNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			EThunderNiagara,
			SpawnLocation,
			FRotator::ZeroRotator
		);
	}
	
	// 스킬 종료 타이머
	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	GetWorldTimerManager().SetTimer(
		SkillTimerHandle,
		this,
		&ALPlayerCharacter::EndSkill,
		QMeteorDuration,
		false
	);
}

void ALPlayerCharacter::UseRSkill(const FVector& TargetLocation)
{
}

void ALPlayerCharacter::CancelCurrentAction()
{
	GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);
	GetWorldTimerManager().ClearTimer(SkillTimerHandle);

	Super::CancelCurrentAction();
}

void ALPlayerCharacter::UseSkill(
		ELPlayerSkillSlot SkillSlot,
		const FVector& TargetLocation)
{
	switch (SkillSlot)
	{
	case ELPlayerSkillSlot::Q:
		UseQSkill(TargetLocation);
		break;
		
	case ELPlayerSkillSlot::W:
		UseWSkill(TargetLocation);
		break;
		
	case ELPlayerSkillSlot::E:
		UseESkill(TargetLocation);
		break;
		
	case ELPlayerSkillSlot::R:
		UseRSkill(TargetLocation);
		break;
	}
}
