// Fill out your copyright notice in the Description page of Project Settings.

#include "LPlayerCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

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
			
			if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
			{
				MovementComp->StopMovementImmediately();
			}
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
		
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetHiddenInGame(false, true);
		}
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
	// =========================
	// 1. 캐릭터가 마우스 위치를 바라보게 하기
	// =========================
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

	// =========================
	// 2. 현재 움직임 즉시 정지
	// =========================
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}

	// =========================
	// 3. 상태를 Skill로 변경
	// =========================
	SetCurrentActionState(ELPlayerActionState::Skill);

	// =========================
	// 4. 메테오 생성 위치 계산
	// =========================
	FVector SpawnLocation = TargetLocation;

	// 메테오가 위에서 떨어지는 Niagara라면 높은 위치에서 생성
	SpawnLocation.Z += QMeteorSpawnHeight;

	// =========================
	// 5. 메테오 Niagara 생성
	// =========================
	if (QMeteorNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			QMeteorNiagara,
			SpawnLocation,
			FRotator::ZeroRotator
		);
	}

	// =========================
	// 6. 스킬 종료 타이머
	// =========================
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
}

void ALPlayerCharacter::UseESkill(const FVector& TargetLocation)
{
}

void ALPlayerCharacter::UseRSkill(const FVector& TargetLocation)
{
}

void ALPlayerCharacter::CancelCurrentAction()
{
	// 대쉬로 기본 공격을 끊는 경우 클리어 타이머로 나중에 남아 있는 공격 종료 타이머가 상태를 덮어 쓰지 않도록 하기 위해서 미리 제거하는 것
	GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);

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
