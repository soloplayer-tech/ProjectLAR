// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_CombatComponent.h"


// Sets default values for this component's properties
UOB_CombatComponent::UOB_CombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	CurHP = MaxHP;
}

void UOB_CombatComponent::TakeDamage(float DamageAmount)
{
	CurHP = FMath::Max(0.f, CurHP - DamageAmount);
	
	if (CurHP <= 0.0f)
	{
		// TODO : Die 상태로 변경하는 로직
	}
}


// Called when the game starts
void UOB_CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOB_CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

