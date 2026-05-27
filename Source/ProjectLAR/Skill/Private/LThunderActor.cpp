
#include "LThunderActor.h"

#include "NiagaraFunctionLibrary.h"


ALThunderActor::ALThunderActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);
}

void ALThunderActor::InitializeThunderStorm(const FVector& InCenterLocation)
{
	CenterLocation = InCenterLocation;
	SetActorLocation(CenterLocation);
	
	CurrentStrikeCount = 0;
	
	GetWorldTimerManager().ClearTimer(StrikeTimerHandle);
	
	GetWorldTimerManager().SetTimer(
		StrikeTimerHandle,
		this,
		&ALThunderActor::SpawnThunderStrike,
		StrikeInterval,
		true,
		0.0f
	);	
}

void ALThunderActor::SpawnThunderStrike()
{
	if (!ThunderStrikeNiagara)
	{
		Destroy();
		return;
	}
	
	if (CurrentStrikeCount >= StrikeCount)
	{
		GetWorldTimerManager().ClearTimer(StrikeTimerHandle);
		Destroy();
		return;
	}
	
	const float RandomAngle = FMath::RandRange(0.f, 360.f);
	const float RandomRadius = FMath::Sqrt(FMath::FRand())*StrikeRadius;
	
	const float Radian = FMath::DegreesToRadians(RandomAngle);
	
	FVector StrikeLocation = CenterLocation;
	StrikeLocation.X += FMath::Cos(Radian) * RandomRadius;
	StrikeLocation.Y += FMath::Sin(Radian) * RandomRadius;
	StrikeLocation.Z += StrikeHeightOffset;
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		ThunderStrikeNiagara,
		StrikeLocation,
		FRotator::ZeroRotator
	);
	
	CurrentStrikeCount++;
}



