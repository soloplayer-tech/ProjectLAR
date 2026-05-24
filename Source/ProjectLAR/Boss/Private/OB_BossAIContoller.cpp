// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossAIContoller.h"
#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
#include "OB_LogManager.h"
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
AOB_BossAIContoller::AOB_BossAIContoller()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	LOG_TRACE_INFO();
	
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	SetPerceptionComponent(*PerceptionComp);
	
	SightConfig->SightRadius            = 1500.f;
	SightConfig->LoseSightRadius        = 2000.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	PerceptionComp -> ConfigureSense(*SightConfig);
	PerceptionComp -> SetDominantSense(SightConfig -> GetSenseImplementation());
	PerceptionComp -> OnTargetPerceptionUpdated.AddDynamic(this, &AOB_BossAIContoller::OnPerceptionUpdated);
	
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


// OnGetPawn에서 FSM 컴포넌트 캐싱
void AOB_BossAIContoller::OnPossess(APawn* InPawn)
{
	LOG_TRACE_INFO(TEXT("Call OnPossess"));
	Super::OnPossess(InPawn);
	BossCharacter = Cast<AOB_BossCharacter>(InPawn);
	FSMComp = BossCharacter -> GetFSMComponent();
}

void AOB_BossAIContoller::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	LOG_TRACE_INFO(TEXT("Call OnPerceptionUpdated"));
	
	// Simulus 감각 제대로 들어갔는지 검증
	if (Stimulus.WasSuccessfullySensed())	
	{
		TargetActor = Actor;
		FSMComp -> SetState(EBossBattleState::MOVE);
	}
	else
	{
		TargetActor = nullptr;
		FSMComp -> SetState(EBossBattleState::IDLE);
	}
}

// FSM이 MOVE State 진입 시 호출
void AOB_BossAIContoller::StartMove(AActor* Target)
{
	LOG_TRACE_INFO(TEXT("Call StartMove"));
	MoveToActor(Target, AcceptanceRadius);
}

