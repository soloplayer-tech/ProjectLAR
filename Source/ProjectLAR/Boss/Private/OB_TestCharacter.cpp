// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_TestCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AOB_TestCharacter::AOB_TestCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoomComp"));
	CameraBoomComp -> SetupAttachment(RootComponent);
	
	CameraBoomComp -> TargetArmLength = 170.0f;
	CameraBoomComp -> SocketOffset = FVector(0.f,0.f,0.f);
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	
	if (!tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object, false);
	}
	
	/** 
	 * Character의 로테이션 YAW (도리도리) 가능
	 * CameraBoomComp에 따른 Pawn 로테이션 가능
	 * 캐릭터의 움직임에 따라 회전 방향을 맞출것인가? -> 불가능 
	 * Reason -> 캐릭터가 회전을 점유해서 가져가게 되면 문제가 될 수 있기 때문에 
	 */
	bUseControllerRotationYaw = true;
	CameraBoomComp -> bUsePawnControlRotation = true;
	GetCharacterMovement() -> bOrientRotationToMovement = false;
	
}

// Called when the game starts or when spawned
void AOB_TestCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
	
}

// Called every frame
void AOB_TestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AOB_TestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

