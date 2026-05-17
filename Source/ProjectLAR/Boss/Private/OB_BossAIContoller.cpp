// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_BossAIContoller.h"
#include "OB_BossCharacter.h"

/* TODO:
 * 1. 상태별 동작 정의
 * 2. 테스트 대상 객체(Dummy Player) 생성
 * 3. 엔진에서 BP 생성 및 동작 테스트
 * 4. 실체 충돌 담당 무기 개발 
 */

// Sets default values
AOB_BossAIContoller::AOB_BossAIContoller()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
void AOB_BossAIContoller::OnGetPawn(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	BossCharacter = Cast<AOB_BossCharacter>(InPawn);
	FSMComp = BossCharacter -> GetFSMComponent();
}

// FSM이 MOVE State 진입 시 호출
void AOB_BossAIContoller::StartMove(AActor* Target)
{
	UE_LOG(LogTemp, Log, TEXT("Call MoveToActor"));
	MoveToActor(Target, AcceptanceRadius);
}

