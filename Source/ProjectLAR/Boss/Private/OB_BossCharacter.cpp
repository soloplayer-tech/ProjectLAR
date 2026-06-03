// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossCharacter.h"

#include "OB_BossFSMComponent.h"
#include "OB_CombatComponent.h"
#include "OB_LogManager.h"
#include "OB_PatternComponent.h"
#include "LFloatingDamageActor.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AOB_BossCharacter::AOB_BossCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	LOG_TRACE_INFO();
	
	FSMComponent = CreateDefaultSubobject<UOB_BossFSMComponent>(TEXT("FSMComponent"));
	PatternComponent = CreateDefaultSubobject<UOB_PatternComponent>(TEXT("PatternComponent"));
	CombatComponent = CreateDefaultSubobject<UOB_CombatComponent>(TEXT("CombatComponent"));
	
	// AutoPossessAI로 컨트롤러 자동 연결
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	GetCharacterMovement() -> bOrientRotationToMovement = true;
	GetCharacterMovement() -> RotationRate = FRotator(0.0f, 360.0f, 0.0f); // 초당 회전 속도
	GetCharacterMovement() -> MaxWalkSpeed = 400.f;
	bUseControllerRotationYaw = false;
}


// Called when the game starts or when spawned
void AOB_BossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOB_BossCharacter::ReceiveSkillDamage_Implementation(float Damage, AActor* DamageCauser, ELPlayerSkillID SkillID)
{
	ILDamageable::ReceiveSkillDamage_Implementation(Damage, DamageCauser, SkillID);
	
	LOG_TRACE_INFO("ReceiveSkillDamage_Implementation");
	
	CombatComponent -> TakeDamage(Damage);
	
	if (FloatingDamageActorClass)
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += FloatingDamageHeightOffset;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		ALFloatingDamageActor* FloatingDamageActor =
			GetWorld()->SpawnActor<ALFloatingDamageActor>(
				FloatingDamageActorClass,
				SpawnLocation,
				FRotator::ZeroRotator,
				SpawnParams
			);

		if (FloatingDamageActor)
		{
			FloatingDamageActor->InitializeFloatingDamage(Damage);
		}
	}
}

// Called every frame
void AOB_BossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AOB_BossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	
}



