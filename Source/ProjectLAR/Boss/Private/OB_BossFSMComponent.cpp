// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossFSMComponent.h"
#include "OB_BossAIController.h"
#include "OB_BossCharacter.h"
#include "OB_CombatComponent.h"
#include "OB_LogManager.h"

// Sets default values for this component's properties
UOB_BossFSMComponent::UOB_BossFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = false; // 틱 계산 X, 이벤트 단위로 계산 진행

	// ...
}

void UOB_BossFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	
	LOG_TRACE_INFO();
	
	Boss = Cast<AOB_BossCharacter>(GetOwner());
	
	if (Boss)
	{
		OwnerController = Cast<AOB_BossAIController>(Boss->GetController());
		CombatComp = Boss -> GetCombatComponent();
	}
	
}

// Called every frame
void UOB_BossFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}


// 상태 진입 시 행동 정의 
void UOB_BossFSMComponent::OnEnterState(EBossBattleState BossState)
{ // 상태 들어감
	
	LOG_TRACE_INFO(TEXT("[ Enter State : %s ]"),*UEnum::GetValueAsString(BossState));
	
	switch (BossState)
	{
		case EBossBattleState::	IDLE:		if (OwnerController) { OwnerController -> StopMovement(); }		 break;
		case EBossBattleState::	MOVE:		if (OwnerController) { OwnerController -> StartMove(); }		 break;
		case EBossBattleState::	ATTACK:		if (OwnerController) { CombatComp	   -> StartAttack(); }		 break;
		case EBossBattleState:: DEAD:		if (OwnerController) { OwnerController -> StopMovement(); }		 break;
		case EBossBattleState::	STUNNED:	break;	
	}
}

void UOB_BossFSMComponent::SetState(EBossBattleState NewState)
{ // 상태 지정
	if (CurAIState == NewState) return;
	// OnExitState(CurAIState); // TODO : ExitState로 상태 해제 관리할 요소가 없어 주석 -> 향후 추가 시 내용 재정의
	
	CurAIState = NewState;
	
	LOG_TRACE_INFO(TEXT("[ Set Current State : %s]"), *UEnum::GetValueAsString(CurAIState));
	
	OnEnterState(CurAIState);
}

void UOB_BossFSMComponent::SetOwnerController(AOB_BossAIController* Controller)
{ // 컨트롤러의 주인 지정
	if (OwnerController == Controller) return;
	
	OwnerController = Controller;
	
	LOG_TRACE_INFO("[ Set OwnerController : %s]", *OwnerController->GetName());
}

void UOB_BossFSMComponent::SetTargetActor(AActor* Actor)
{ // 타겟 지정
	if (Actor == nullptr) { LOG_TRACE_WARN(TEXT("Actor is nullptr")); return; }
	
	Target = Actor;
	
	LOG_TRACE_INFO(TEXT("[ Set Target : %s]"), *Target->GetName());
}

