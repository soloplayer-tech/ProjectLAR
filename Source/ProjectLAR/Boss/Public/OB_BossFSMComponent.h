// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "States/OB_EBossBattleState.h"
#include "OB_BossFSMComponent.generated.h"

/**FSM Component
 * @brief '상태를 누가 관리하지?' → FSM 컴포넌트.
 */

enum class EAttackState : uint8;
class AOB_BossCharacter;
class AOB_BossAIController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTLAR_API UOB_BossFSMComponent : public UActorComponent
{
	GENERATED_BODY()
	
	// 보스 열겨형 호출
	UPROPERTY()
	EBossBattleState CurAIState = EBossBattleState::IDLE;			// 현재 상태 저장 변수
	
	// UAIFSMComponent.h
	UPROPERTY()
	TObjectPtr<AOB_BossAIController> OwnerController;
	
	UPROPERTY()
	TObjectPtr<AOB_BossCharacter> Boss;
	
	UPROPERTY()
	TObjectPtr<AActor> Target;


public:
	// Sets default values for this component's properties
	UOB_BossFSMComponent();
	
	void SetOwnerController(AOB_BossAIController* OwnerController);
	void SetTargetActor(AActor* Actor);
	void SetState(EBossBattleState NewState);						// 상태 변경
	void OnEnterState(EBossBattleState BossState);					// 상태 ENTER 정의
    	// void OnExitState(EBossState BossState);						// TODO: 상태 EXIT 정의 현재 개발 단계상 불필요
	
	TObjectPtr<AActor> GetTargetActor() const { return Target; }
	EBossBattleState GetCurState() const { return CurAIState; }
	
	void SetAttackPattern();
	void OnEnterAtkPattern(EAttackState AttackPattern);

protected:
	// Called when the game starts
	virtual auto BeginPlay() -> void override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
