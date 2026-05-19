// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "States/OB_EBossAIState.h"
#include "OB_BossFSMComponent.generated.h"

/**FSM Component
 * @brief '상태를 누가 관리하지?' → FSM 컴포넌트.
 */

class AOB_BossAIContoller;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTLAR_API UOB_BossFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOB_BossFSMComponent();
	
	// 보스 열겨형 호출
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossFSM")
	EBossAIState CurAIState = EBossAIState::IDLE;						// 현재 상태 저장 변수
	
	// UAIFSMComponent.h
	UPROPERTY()
	TSubclassOf<AOB_BossAIContoller> OwnerController;

	void SetState(EBossAIState NewState);								// 상태 변경
	void OnEnterState(EBossAIState BossState);						// 상태 ENTER 정의
	// void OnExitState(EBossState BossState);						// TODO: 상태 EXIT 정의 현재 개발 단계상 불필요

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
