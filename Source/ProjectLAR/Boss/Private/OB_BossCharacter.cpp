// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
#include "OB_LogManager.h"
#include "OB_CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AOB_BossCharacter::AOB_BossCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	LOG_TRACE_INFO();
	
	FSMComponent = CreateDefaultSubobject<UOB_BossFSMComponent>(TEXT("FSMComponent"));
	
	// AutoPossessAI로 컨트롤러 자동 연결
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	GetCharacterMovement() -> bOrientRotationToMovement = true;
	GetCharacterMovement() -> RotationRate = FRotator(0.0f, 360.0f, 0.0f); // 초당 회전 속도
	bUseControllerRotationYaw = false;
	
	CurHP = MaxHP;
}

// Called when the game starts or when spawned
void AOB_BossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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



void AOB_BossCharacter::ReceiveSkillDamage_Implementation(
	float Damage,
	AActor* DamageCauser,
	ELPlayerSkillID SkillID
)
{
	if (CurHP <= 0.0f)
	{
		return;
	}

	CurHP -= Damage;
	CurHP = FMath::Max(0.0f, CurHP);
	
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
	
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("TestDamageBox Hit! Skill: %s / Damage: %.1f / HP: %.1f / %.1f / Causer: %s"),
		*UEnum::GetValueAsString(SkillID),
		Damage,
		CurHP,
		MaxHP,
		DamageCauser ? *DamageCauser->GetName() : TEXT("None")
	);

	if (CurHP <= 0.0f)
	{
		Die();
	}
}

void AOB_BossCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Dead"));

	if (bDestroyOnDeath)
	{
		Destroy();
	}
}
