// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossFSMComponent.h"
#include "OB_BossAIController.h"
#include "OB_BossCharacter.h"

#include "OB_LogManager.h"
#include "States/OB_EAttackPattern.h"

// Sets default values for this component's properties
UOB_BossFSMComponent::UOB_BossFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = false; // 틱 계산 X, 이벤트 단위로 계산 진행
	LOG_TRACE_INFO();

	// ...
}

void UOB_BossFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UOB_BossFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Boss = Cast<AOB_BossCharacter>(GetOwner());
	
	if (Boss)
	{
		OwnerController = Cast<AOB_BossAIController>(Boss->GetController());
	}
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
		case EBossBattleState::	ATTACK:		if (OwnerController) { SetAttackPattern(); }					 break;
		case EBossBattleState::	STUNNED:	break;	
	}
}

void UOB_BossFSMComponent::OnEnterAtkPattern(EAttackState AttackPattern)
{
	LOG_TRACE_INFO(TEXT("[ Enter State : %s ]"),*UEnum::GetValueAsString(AttackPattern))
	
	switch (AttackPattern)
	{
	case EAttackState::HAMMER:	 	if ( OwnerController ) { /* TODO: Hammer Pattern */ }	break;
	case EAttackState::RUSH:		if ( OwnerController ) { /* TODO: RUSH Pattern */ }		break;
	case EAttackState::SLAM:		if ( OwnerController ) { /* TODO: SLAM Pattern */ }		break;
	case EAttackState::CARD:		if ( OwnerController ) { /* TODO: CARD Pattern */ }		break;
	}
}

// TODO : Random 상태값으로 데이터를 지정. 추후 타겟과 보스의 상태에 따라 변화가능하게 변경.
void UOB_BossFSMComponent::SetAttackPattern()
{
	if (CurAIState != EBossBattleState::ATTACK) return;
	
	int32 RandNum = FMath::RandRange(0, 3);
	
	EAttackState CurAtkPattern = EAttackState::HAMMER;
	
	switch (RandNum)
	{
		case 0: CurAtkPattern = EAttackState::HAMMER;		break;
		case 1: CurAtkPattern = EAttackState::RUSH;			break;
		case 2: CurAtkPattern = EAttackState::SLAM;			break;
		case 3: CurAtkPattern = EAttackState::CARD;			break;
	
		default:											break;
	}
	
	LOG_TRACE_INFO(TEXT("[Set Current State : %s"), *UEnum::GetValueAsString(CurAtkPattern));
	
	OnEnterAtkPattern(CurAtkPattern);
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

