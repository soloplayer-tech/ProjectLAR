// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OB_CombatComponent.generated.h"

/**
 * TODO : 
 * - [ ] : Distance 구하기 
 * - [ ] : Slam 기능 구현
 * - [ ] : Hammer 기능 구현
 * - [ ] : Rush 기능 구현
 * - [ ] : Card 기능 구현
 * - [ ] : CharacterHP 받아서 State 분기 처리 -- 현재는 일단 랜덤 값으로 계산
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTLAR_API UOB_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOB_CombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
