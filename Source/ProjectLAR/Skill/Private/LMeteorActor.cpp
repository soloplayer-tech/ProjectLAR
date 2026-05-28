#include "LMeteorActor.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "LPlayerSkillID.h"
#include "Engine/OverlapResult.h"

ALMeteorActor::ALMeteorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	MeteorNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MeteorNiagaraComp"));
	MeteorNiagaraComp->SetupAttachment(RootScene);
}

void ALMeteorActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALMeteorActor::InitializeMeteor(const FVector& InImpactLocation)
{
	ImpactLocation = InImpactLocation;

	StartLocation = ImpactLocation;
	StartLocation.Z += MeteorStartHeight;

	SetActorLocation(StartLocation);

	ElapsedTime = 0.0f;
	bInitialized = true;
	bImpactHandled = false;
	// 바닥 경고 장판
	if (MeteorWarningNiagara)
	{
		WarningNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			MeteorWarningNiagara,
			ImpactLocation,
			FRotator::ZeroRotator
		);
	}
}

void ALMeteorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bInitialized || bImpactHandled)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	const float SafeFallDuration = FMath::Max(0.01f, FallDuration);
	
	const float Alpha = FMath::Clamp(
		ElapsedTime / FallDuration,
		0.0f,
		1.0f
	);

	const FVector NewLocation = FMath::Lerp(
		StartLocation,
		ImpactLocation,
		Alpha
	);

	SetActorLocation(NewLocation);

	if (Alpha >= 1.0f)
	{
		Impact();
	}
}

void ALMeteorActor::Impact()
{
	// 장판 제거
	if (WarningNiagaraComp)
	{
		WarningNiagaraComp->DestroyComponent();
		WarningNiagaraComp = nullptr;
	}

	// 착탄 폭발 이펙트
	if (ImpactNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactNiagara,
			ImpactLocation,
			FRotator::ZeroRotator
		);
	}

	// 나중에 여기서 범위 데미지 처리
	// ApplyRadialDamage or SphereOverlapActors
	ApplyImpactDamage();
	
	Destroy();
}

void ALMeteorActor::ApplyImpactDamage()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	const FVector DamageLocation = ImpactLocation;

	if (bDrawImpactDebug)
	{
		DrawDebugSphere(
			World,
			DamageLocation,
			ImpactDamageRadius,
			24,
			FColor::Red,
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
		DamageLocation,
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

void ALMeteorActor::ApplyDamageToActor(AActor* TargetActor)
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
		MeteorDamage,
		DamageCauser,
		ELPlayerSkillID::Meteor
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Meteor Impact Damage: %s / Damage: %.1f"),
		*TargetActor->GetName(),
		MeteorDamage
	);
}