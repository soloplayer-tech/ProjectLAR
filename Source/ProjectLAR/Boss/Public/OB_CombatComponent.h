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
	
	// TODO: EAttackState GetFitAttackPattern(float Distance); 추후 디테일 작업들어갈 떄 개발
	
	void TakeDamage(float DamageAmount);
	float GetCurHP() const { return CurHP };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Boss | Stats")
	float MaxHP = 1000.0f;
	
	float CurHP;
	
	// RUSH 전술 수치
	UPROPERTY(EditAnywhere, Category = "Boss | Tactics")
	float RushTriggerDistance = 600.0f; // 6미터 이상이면 돌진

	// RUSH 연속 발동을 막기 위한 내부 쿨타임 제어 변수
	bool bIsRushCooldown = false;
	FTimerHandle RushCooldownTimerHandle;
	

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
