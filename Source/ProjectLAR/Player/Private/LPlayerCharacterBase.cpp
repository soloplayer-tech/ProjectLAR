#include "./../Public/LPlayerCharacterBase.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
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
	
}

void ALPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ALPlayerCharacterBase::Dash(const FVector& DashDirection)
{
	FVector FinalDashDirection = DashDirection;
	FinalDashDirection.Z = 0.0f;
	FinalDashDirection.Normalize();
	
	if (FinalDashDirection.IsNearlyZero())
	{
		return;
	}
	
	// 대쉬 방향을 바라보게 만들고 싶다.
	const FRotator DashRotation = FinalDashDirection.Rotation();
	SetActorRotation(FRotator(0.0f, DashRotation.Yaw, 0.0f));
	
	
	LaunchCharacter(
		DashDirection* DashPower,
		true,
		true
		);
}

