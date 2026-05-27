#include "ProjectLAR/Skill/Public/LIceLanceActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "LPlayerSkillID.h"
#include "Engine/OverlapResult.h"

ALIceLanceActor::ALIceLanceActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	IceLanceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IceLanceMesh"));
	IceLanceMesh->SetupAttachment(RootScene);

	// 얼음창은 비행 중 충돌로 데미지를 주지 않는다.
	// 착탄 순간 SphereOverlap으로만 데미지를 준다.
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
	const FVector MoveDirection = NewLocation - PreviousLocation;

	if (!MoveDirection.IsNearlyZero())
	{
		SetActorRotation(MoveDirection.Rotation());
	}
	
	// 목적지 도착 순간에만 데미지 적용 후 제거
	if (T >= 1.0f)
	{
		ApplyImpactDamage();
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

	TravelDuration = FMath::Max(0.01f, InTravelDuration);
	FireDelay = InFireDelay;

	ElapsedTime = 0.0f;
	WaitElapsedTime = 0.0f;

	bPathInitialized = true;
	bIsFlying = FireDelay <= 0.0f;
	bImpactDamageApplied = false;

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

void ALIceLanceActor::ApplyImpactDamage()
{
	if (bImpactDamageApplied)
	{
		return;
	}

	bImpactDamageApplied = true;

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	const FVector ImpactLocation = GetActorLocation();

	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			ImpactEffect,
			ImpactLocation,
			GetActorRotation()
		);
	}

	if (bDrawImpactDebug)
	{
		DrawDebugSphere(
			World,
			ImpactLocation,
			ImpactDamageRadius,
			16,
			FColor::Cyan,
			false,
			1.0f
		);
	}

	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetOwner())
	{
		QueryParams.AddIgnoredActor(GetOwner());
	}

	const bool bHit = World->OverlapMultiByObjectType(
		OverlapResults,
		ImpactLocation,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(ImpactDamageRadius),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	TArray<AActor*> DamagedActors;

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* HitActor = Result.GetActor();

		if (!HitActor)
		{
			continue;
		}

		if (DamagedActors.Contains(HitActor))
		{
			continue;
		}

		DamagedActors.Add(HitActor);

		ApplyDamageToActor(HitActor);
	}
}

void ALIceLanceActor::ApplyDamageToActor(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}

	if (!TargetActor->GetClass()->ImplementsInterface(ULDamageable::StaticClass()))
	{
		return;
	}

	AActor* DamageCauser = GetOwner();

	if (!DamageCauser)
	{
		DamageCauser = this;
	}

	ILDamageable::Execute_ReceiveSkillDamage(
		TargetActor,
		IceLanceDamage,
		DamageCauser,
		ELPlayerSkillID::IceLance
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("IceLance Impact Damage: %s / Damage: %.1f"),
		*TargetActor->GetName(),
		IceLanceDamage
	);
}