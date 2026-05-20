#include "./../Public/LIceLanceActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ALIceLanceActor::ALIceLanceActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	IceLanceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IceLanceMesh"));
	IceLanceMesh->SetupAttachment(RootScene);

	// 지금은 이동용 액터만 만들 것이므로
	// 충돌은 나중에 데미지 구현할 때 붙여도 됨
	IceLanceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALIceLanceActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALIceLanceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bPathInitialized)
	{
		return;
	}

	
	// 아직 발사 전이면 공중에서 대기
	
	if (!bIsFlying)
	{
		WaitElapsedTime += DeltaTime;

		if (WaitElapsedTime < FireDelay)
		{
			return;
		}

		// 대기 끝 → 비행 시작
		bIsFlying = true;
		ElapsedTime = 0.0f;
	}

	
	// 베지어 곡선 이동
	
	ElapsedTime += DeltaTime;

	const float T = FMath::Clamp(
		ElapsedTime / TravelDuration,
		0.0f,
		1.0f
	);

	const FVector PreviousLocation = GetActorLocation();
	const FVector NewLocation = GetQuadraticBezierPoint(T);

	SetActorLocation(NewLocation);

	
	// 실제 이동 방향을 바라보도록 회전
	//    Z를 지우지 않으므로 위아래 각도도 따라감
	
	const FVector MoveDirection = NewLocation - PreviousLocation;

	if (!MoveDirection.IsNearlyZero())
	{
		const FRotator NewRotation = MoveDirection.Rotation();
		SetActorRotation(NewRotation);
	}
	
	// 목적지 도착 시 제거
	
	if (T >= 1.0f)
	{
		Destroy();
	}
}

void ALIceLanceActor::InitializeBezierPath(
	const FVector& InStartPoint,
	const FVector& InControlPoint,
	const FVector& InEndPoint,
	float InTravelDuration,
	float InFireDelay
)
{
	StartPoint = InStartPoint;
	ControlPoint = InControlPoint;
	EndPoint = InEndPoint;

	TravelDuration = InTravelDuration;
	FireDelay = InFireDelay;

	ElapsedTime = 0.0f;
	WaitElapsedTime = 0.0f;

	bPathInitialized = true;
	bIsFlying = FireDelay <= 0.0f;

	SetActorLocation(StartPoint);

	// 생성된 순간부터 첫 곡선 방향을 바라보게 함
	const FVector InitialDirection = ControlPoint - StartPoint;

	if (!InitialDirection.IsNearlyZero())
	{
		SetActorRotation(InitialDirection.Rotation());
	}
}

FVector ALIceLanceActor::GetQuadraticBezierPoint(float T) const
{
	const float OneMinusT = 1.0f - T;

	return
		OneMinusT * OneMinusT * StartPoint
		+ 2.0f * OneMinusT * T * ControlPoint
		+ T * T * EndPoint;
}