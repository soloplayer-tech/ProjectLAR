#include "LPlayerCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


ALPlayerCharacterBase::ALPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoomComp"));
	CameraBoomComp->SetupAttachment(RootComponent);
	
	CameraBoomComp->TargetArmLength = 1500.f;
	CameraBoomComp->SocketOffset = FVector(0.f, -55.f, 0.f); 
	CameraBoomComp->bDoCollisionTest = false;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(FName("CameraComp"));
	CameraComp->SetupAttachment(CameraBoomComp);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	
	/*PlayerHPWidget = CreateDefaultSubobject<UWidgetComponent>(FName("PlayerHPWidget"));
	PlayerHPWidget->SetupAttachment(RootComponent);*/
	
	
	// 캐릭터가 컨트롤러 회전을 강제로 따라가지 않게
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void ALPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (CameraBoomComp)
	{
		TargetCameraArmLength = CameraBoomComp->TargetArmLength;
	}
}

void ALPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDash(DeltaTime);
	UpdateCameraZoom(DeltaTime);
}

void ALPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ALPlayerCharacterBase::CameraZoomIn()
{
	// 휠 다운 시 호출
	// TargetArmLength가 줄어들면 카메라가 가까워진다.
	TargetCameraArmLength = FMath::Clamp(
		TargetCameraArmLength + CameraZoomStep,
		MinCameraArmLength,
		MaxCameraArmLength
	);
}

void ALPlayerCharacterBase::CameraZoomOut()
{
	// 휠 업 시 호출
	// TargetArmLength가 늘어나면 카메라가 멀어진다.
	TargetCameraArmLength = FMath::Clamp(
		TargetCameraArmLength - CameraZoomStep,
		MinCameraArmLength,
		MaxCameraArmLength
	);
}

void ALPlayerCharacterBase::UpdateCameraZoom(float DeltaTime)
{
	if (!CameraBoomComp)
	{
		return;
	}

	CameraBoomComp->TargetArmLength = FMath::FInterpTo(
		CameraBoomComp->TargetArmLength,
		TargetCameraArmLength,
		DeltaTime,
		CameraZoomInterpSpeed
	);
}

void ALPlayerCharacterBase::Dash(const FVector& DashDirection)
{
	if (!CanDash())
	{
		return;
	}
	
	FVector FinalDashDirection = DashDirection;
	FinalDashDirection.Z = 0.f;
	
	if (!FinalDashDirection.Normalize())
	{
		return;
	}
	
	const FRotator DashRotation = FinalDashDirection.Rotation();
	
	SetActorRotation(
		FRotator(
			0.0f,
			DashRotation.Yaw,
			0.0f
		)
	);
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	CurrentActionState = ELPlayerActionState::Dash;
	
	bIsDashing = true;
	bCanDash = false;
	DashElapsedTime = 0.f;
	
	DashStartLocation = GetActorLocation();
	DashTargetLocation  = DashStartLocation + FinalDashDirection * DashDistance;
}

void ALPlayerCharacterBase::UpdateDash(float DeltaTime)
{
	if (!bIsDashing)
	{
		return;
	}
	
	DashElapsedTime += DeltaTime;
	
	const float Alpha = FMath::Clamp(
		DashElapsedTime / DashDuration,
		0.0f,
		1.0f
		);
	
	// 살짝 부드러운 보간
	const float SmoothAlpha = FMath::InterpEaseOut(
		0.0f,
		1.0f,
		Alpha,
		2.0f
		);
	
	const FVector NewLocation = FMath::Lerp(
		DashStartLocation,
		DashTargetLocation,
		SmoothAlpha
		);
	
	FHitResult HitResult;
	
	SetActorLocation(NewLocation, true, &HitResult);
	
	// 벽에 막히면 대쉬 종료
	if (HitResult.bBlockingHit)
	{
		EndDash();
		return;
	}
	
	if (Alpha >= 1.0f)
	{
		EndDash();
	}
}

void ALPlayerCharacterBase::EndDash()
{
	if (!bIsDashing)
	{
		return;
	}
	
	bIsDashing = false;
	DashElapsedTime = 0.f;
	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
	}
	
	CurrentActionState = ELPlayerActionState::Idle;
	
	GetWorldTimerManager().ClearTimer(DashCooldownTimerHandle);
	
	GetWorldTimerManager().SetTimer(
		DashCooldownTimerHandle,
		this,
		&ALPlayerCharacterBase::ResetDashCooldown,
		DashCooldown,
		false
		);
}

void ALPlayerCharacterBase::ResetDashCooldown()
{
	bCanDash = true;
}

ELPlayerActionState ALPlayerCharacterBase::GetCurrentActionState() const
{
	return CurrentActionState;
}

void ALPlayerCharacterBase::SetCurrentActionState(ELPlayerActionState NewState)
{
	CurrentActionState = NewState;
}

bool ALPlayerCharacterBase::CanMove() const
{
	return CurrentActionState == ELPlayerActionState::Idle 
		|| CurrentActionState == ELPlayerActionState::Casting;
	// 기본 상태 + 캐스팅 시전시간에만 에서만 이동 가능
}

bool ALPlayerCharacterBase::CanBasicAttack() const
{
	return CurrentActionState == ELPlayerActionState::Idle;
	// 기본 상태에서만 베이직 어택 가능
}

bool ALPlayerCharacterBase::CanDash() const
{
	return bCanDash && !bIsDashing 
	&& (
		CurrentActionState == ELPlayerActionState::Idle	
		|| CurrentActionState == ELPlayerActionState::BasicAttack
		|| CurrentActionState == ELPlayerActionState::Skill
		|| CurrentActionState == ELPlayerActionState::Casting
		);
	// 기본 상태 + 기본 공격 + 스킬 상태 중에도 대쉬 가능
}

bool ALPlayerCharacterBase::IsDashing() const
{
	return bIsDashing;
}

bool ALPlayerCharacterBase::IsDashOnCooldown() const
{
	if (!GetWorld())
	{
		return false;
	}

	return GetWorldTimerManager().IsTimerActive(DashCooldownTimerHandle);
}

float ALPlayerCharacterBase::GetDashCooldownRemaining() const
{
	if (!GetWorld())
	{
		return 0.0f;
	}

	if (!GetWorldTimerManager().IsTimerActive(DashCooldownTimerHandle))
	{
		return 0.0f;
	}

	return FMath::Max(
		0.0f,
		GetWorldTimerManager().GetTimerRemaining(DashCooldownTimerHandle)
	);
}

float ALPlayerCharacterBase::GetDashCooldownRatio() const
{
	if (DashCooldown <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(
		GetDashCooldownRemaining() / DashCooldown,
		0.0f,
		1.0f
	);
}


bool ALPlayerCharacterBase::CanUseSkill() const
{
	return CurrentActionState == ELPlayerActionState::Idle;
}

void ALPlayerCharacterBase::CancelCurrentAction()
{
	CurrentActionState = ELPlayerActionState::Idle;
	// 상태를 기본으로 돌린다. 기본공격 할 때
}
