#include "LMeteorActor.h"

#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

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

	if (!bInitialized)
	{
		return;
	}

	ElapsedTime += DeltaTime;

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

	Destroy();
}