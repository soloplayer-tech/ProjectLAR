// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossAIContoller.h"


// Sets default values
AOB_BossAIContoller::AOB_BossAIContoller()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOB_BossAIContoller::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOB_BossAIContoller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

