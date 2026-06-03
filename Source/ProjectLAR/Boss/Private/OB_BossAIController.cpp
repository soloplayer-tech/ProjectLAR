// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossAIController.h"

#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
#include "OB_LogManager.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// 카테고리 정의 

/* TODO: 실제로 캐릭터를 움직이는 건 누구지?' → AIController. 상태변경 시 MoveToActor 실행
 * - [ ] : 1. 상태별 동작 정의
 * - [O] : 2. 테스트 대상 객체(Dummy Player) 생성
 * - [O] : 3. 엔진에서 BP 생성 및 동작 테스트
 * - [ ] : 4. 실체 충돌 담당 무기 개발
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
	
	SightConfig->SightRadius            = Sight_Radius;
	SightConfig->LoseSightRadius        = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	PerceptionComp -> ConfigureSense(*SightConfig);
	PerceptionComp -> SetDominantSense(SightConfig -> GetSenseImplementation());
	
	// 월드와 네비게이션 시스템 가져오기
	World = GetWorld();
	NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	
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
	LOG_TRACE_INFO();

	if (Actor == nullptr)
	{
		LOG_TRACE_WARN(TEXT("Target Actor is nullptr"));
		return;
	}
	
	// Stimulus 감각 제대로 들어갔는지 검증
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
				
				if (FSMComp->GetCurState() == EBossBattleState::IDLE && BossCharacter)
				{
					OnFindTarget(Actor);	
				} else
				{
					FSMComp -> SetState(EBossBattleState::MOVE);
				}
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
	LOG_TRACE_INFO();
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(FSMComp->GetTargetActor());
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	MoveRequest.SetCanStrafe(false);
	
	FPathFollowingRequestResult  MoveResult = MoveTo(MoveRequest);
	
	switch (MoveResult.Code.GetValue())
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

void AOB_BossAIController::OnFindTarget(AActor* TargetActor)
{
	LOG_TRACE_INFO(TEXT("[Call OnFindTarget Target : %s]"), *TargetActor -> GetName());
	
	if (!BossCharacter) { LOG_TRACE_WARN("BossCharacter is Null !!"); return; }
	
	FVector TargetLocation = TargetActor -> GetActorLocation();
	FVector BossLocation = BossCharacter -> GetActorLocation();
	
	if ( FVector::Dist(TargetLocation,BossLocation) > LoseSightRadius )
	{
		TeleportRandomlyAroundTarget(TargetLocation);
		FSMComp -> SetState(EBossBattleState::MOVE);
	}
}


void AOB_BossAIController::TeleportRandomlyAroundTarget(const FVector& TargetLocation)
{
	LOG_TRACE_INFO();
	
	if (TargetLocation.IsZero()) { LOG_TRACE_WARN("TargetLocation is (0, 0, 0)"); return; }
	if (!World || !NavSystem) { LOG_TRACE_WARN("World || NavSystem is nullptr"); return; }

	FNavLocation RandomNavLocation;
	
	if (FindSafetyLocation(TargetLocation,RandomNavLocation))
	{
		SetBossLocation(TargetLocation,RandomNavLocation);
	} // 괜찮은 장소가 있다면 그곳으로 이동. 
	
}

bool AOB_BossAIController::FindSafetyLocation(const FVector& TargetLocation, FNavLocation& SafetyLocation)
{
	LOG_TRACE_INFO();
	// 안전한 무작위 위치를 찾을 때까지 반복 (10번 정도);
	for (int32 i = 0; i < 10; ++i)
	{
		if (NavSystem->GetRandomReachablePointInRadius(TargetLocation, MaxRadius, SafetyLocation))
		{
			float Distance = FVector::Dist(TargetLocation, SafetyLocation.Location);
			if (Distance >= MinRadius) // 플레이어랑 너무 가깝지 않게 조정 (MinRadius)
			{
				return true;
			}
		}
	}
	
	return false;
}

void AOB_BossAIController::SetBossLocation(const FVector& TargetLocation, const FNavLocation& RandomNavLocation)
{
	LOG_TRACE_INFO();
	
	if (!BossCharacter) { LOG_TRACE_WARN("BossCharacter is Null !!"); return; }
	
	BossCharacter -> SetActorLocation(RandomNavLocation.Location, false, nullptr, ETeleportType::TeleportPhysics);
        
	FVector LookDirection = TargetLocation - RandomNavLocation.Location;
	FRotator NewRotation = LookDirection.Rotation();
	NewRotation.Pitch = 0.f; 
	NewRotation.Roll = 0.f;
	BossCharacter -> SetActorRotation(NewRotation);
}
