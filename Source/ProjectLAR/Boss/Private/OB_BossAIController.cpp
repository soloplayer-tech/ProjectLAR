// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossAIController.h"
#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
#include "OB_LogManager.h"

#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// 카테고리 정의 

/* TODO: 실제로 캐릭터를 움직이는 건 누구지?' → AIController. 상태변경 시 MoveToActor 실행
 * 1. 상태별 동작 정의
 * 2. 테스트 대상 객체(Dummy Player) 생성
 * 3. 엔진에서 BP 생성 및 동작 테스트
 * 4. 실체 충돌 담당 무기 개발
 */

// Sets default values
AOB_BossAIController::AOB_BossAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	LOG_TRACE_INFO();
	
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	SetPerceptionComponent(*PerceptionComp);
	
	SightConfig->SightRadius            = 1500.f;
	SightConfig->LoseSightRadius        = 2000.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	PerceptionComp -> ConfigureSense(*SightConfig);
	PerceptionComp -> SetDominantSense(SightConfig -> GetSenseImplementation());
	
}

// Called when the game starts or when spawned
void AOB_BossAIController::BeginPlay()
{
	Super::BeginPlay();
	
	PerceptionComp -> OnTargetPerceptionUpdated.AddDynamic(this, &AOB_BossAIController::OnPerceptionUpdated);
}

// Called every frame
void AOB_BossAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// OnGetPawn에서 FSM 컴포넌트 캐싱
void AOB_BossAIController::OnPossess(APawn* InPawn)
{
	LOG_TRACE_INFO(TEXT("Call OnPossess"));
	Super::OnPossess(InPawn);
	
	BossCharacter = Cast<AOB_BossCharacter>(InPawn);
	if (BossCharacter)
	{
		LOG_TRACE_INFO(TEXT("Boss Character Exist"));
		FSMComp = BossCharacter->GetFSMComponent();
		if (FSMComp)
		{
			LOG_TRACE_INFO(TEXT("FSM Exist"));
			FSMComp->SetOwnerController(this);  // FSM한테 직접 전달
		}
	
	}
}

void AOB_BossAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	LOG_TRACE_INFO(TEXT("Call OnPerceptionUpdated"));

	if (Actor == nullptr)
	{
		LOG_TRACE_WARN(TEXT("Target Actor is nullptr"));
		return;
	}
	
	
	// Simulus 감각 제대로 들어갔는지 검증
	if (Stimulus.WasSuccessfullySensed())	
	{
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		
		// 타겟이 플레이어와 같은지 TODO : 이후 멀티 혹은 다른 개체도 탐색 대상이 될 시 변경 필요
		if (Actor == PlayerCharacter)
		{
			LOG_TRACE_INFO("Checked Player");
			
			if (FSMComp)
			{
				LOG_TRACE_INFO("FSMComp -> SetTargetActor(Actor);");
				FSMComp -> SetTargetActor(Actor);
				FSMComp -> SetState(EBossBattleState::MOVE);
			}
		}
	}
	else
	{
		if (FSMComp && Actor == FSMComp -> GetTargetActor()) 
		{
			FSMComp -> SetState(EBossBattleState::IDLE);
		}
	}
}

	
// FSM이 MOVE State 진입 시 호출
void AOB_BossAIController::StartMove()
{
	LOG_TRACE_INFO(TEXT("Call StartMove"));
	
	switch (MoveToActor(FSMComp->GetTargetActor(), AcceptanceRadius))
	{
	case EPathFollowingRequestResult::AlreadyAtGoal:
		FSMComp -> SetState(EBossBattleState::ATTACK);
		break;
		
	case EPathFollowingRequestResult::Failed:
		FSMComp->SetState(EBossBattleState::IDLE);
		break;

	case EPathFollowingRequestResult::RequestSuccessful:
		FSMComp->SetState(EBossBattleState::MOVE);
		break;
	}
}
