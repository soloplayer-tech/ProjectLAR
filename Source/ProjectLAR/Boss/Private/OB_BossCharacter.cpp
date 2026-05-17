// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectLAR/Boss/Public/OB_BossCharacter.h"


// Sets default values
AOB_BossCharacter::AOB_BossCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FSMComponent = CreateDefaultSubobject<UOB_BossFSMComponent>(TEXT("FSMComponent"));
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


