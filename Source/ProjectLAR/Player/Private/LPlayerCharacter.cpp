// Fill out your copyright notice in the Description page of Project Settings.

#include "./../Public/LPlayerCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ALPlayerCharacter::ALPlayerCharacter()
{
	bBlink = true;
}

void ALPlayerCharacter::Dash(const FVector& DashDirection)
{
	if (bBlink)
	{
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

void ALPlayerCharacter::EndBlink()
{
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

void ALPlayerCharacter::CancelCurrentAction()
{
	// 대쉬로 기본 공격을 끊는 경우 클리어 타이머로 나중에 남아 있는 공격 종료 타이머가 상태를 덮어 쓰지 않도록 하기 위해서 미리 제거하는 것
	GetWorldTimerManager().ClearTimer(BasicAttackTimerHandle);

	Super::CancelCurrentAction();
}
