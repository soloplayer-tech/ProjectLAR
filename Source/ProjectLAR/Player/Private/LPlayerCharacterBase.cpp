#include "./../Public/LPlayerCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


ALPlayerCharacterBase::ALPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoomComp"));
	CameraBoomComp->SetupAttachment(RootComponent);
	
	CameraBoomComp->TargetArmLength = 1500.f;
	CameraBoomComp->SocketOffset = FVector(0.f, -55.f, 0.f); 
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(FName("CameraComp"));
	CameraComp->SetupAttachment(CameraBoomComp);
	
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

