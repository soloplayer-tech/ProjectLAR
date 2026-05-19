// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossFSMComponent.h"


// Sets default values for this component's properties
UOB_BossFSMComponent::UOB_BossFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = false; // 틱 계산 X, 이벤트 단위로 계산 진행

	// ...
}

// Called when the game starts
void UOB_BossFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UOB_BossFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UOB_BossFSMComponent::SetState(EBossAIState NewState)
{
	if (CurAIState == NewState) return;
	// OnExitState(CurAIState); // TODO: ExitState로 상태 해제 관리할 요소가 없어 주석 -> 향후 추가 시 내용 재정의
	
	FString DisplayName = StaticEnum<EBossAIState>()->GetDisplayNameTextByValue((int64) CurAIState).ToString();
	UE_LOG(LogTemp, Warning, TEXT("[FSM] %s"), *DisplayName);

	CurAIState = NewState;
	OnEnterState(CurAIState);
}

// 상태 진입 시 행동 정의 
void UOB_BossFSMComponent::OnEnterState(EBossAIState BossState)
{	
	/* TODO :
	 * 각 상태별 행동 정의 
	 */
	
	UE_LOG(LogTemp, Display, TEXT("[ Enter State : %s ]"), *UEnum::GetValueAsString(BossState));
	
	switch (BossState)
	{
	case EBossAIState::	IDLE:		GetOwner()-> GetInstigatorController() -> StopMovement(); break;
	case EBossAIState::	MOVE:		break;
	case EBossAIState::	ATTACK:		break;
	case EBossAIState::	STUNNED:	break;
	}
}

