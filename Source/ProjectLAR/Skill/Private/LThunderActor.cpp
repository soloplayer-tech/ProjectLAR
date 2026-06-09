
#include "LThunderActor.h"
#include "DrawDebugHelpers.h"
#include "LPlayerCharacter.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "LPlayerSkillID.h"
#include "NiagaraFunctionLibrary.h"


ALThunderActor::ALThunderActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);
}

void ALThunderActor::InitializeThunderStorm(const FVector& InCenterLocation)
{
	InitializeThunderStorm(
		InCenterLocation,
		ThunderDamage,
		ThunderDamageRadius,
		StrikeRadius,
		StrikeCount,
		StrikeInterval,
		StrikeHeightOffset,
		ThunderStrikeNiagara
	);
}

void ALThunderActor::InitializeThunderStorm(
	const FVector& InCenterLocation,
	float InDamage,
	float InDamageRadius,
	float InStrikeRadius,
	int32 InStrikeCount,
	float InStrikeInterval,
	float InStrikeHeightOffset,
	UNiagaraSystem* InThunderStrikeNiagara
)
{
	CenterLocation = InCenterLocation;
	SetActorLocation(CenterLocation);

	ThunderDamage = FMath::Max(0.0f, InDamage);
	ThunderDamageRadius = FMath::Max(1.0f, InDamageRadius);
	StrikeRadius = FMath::Max(0.0f, InStrikeRadius);
	StrikeCount = FMath::Max(0, InStrikeCount);
	StrikeInterval = FMath::Max(0.01f, InStrikeInterval);
	StrikeHeightOffset = InStrikeHeightOffset;

	if (InThunderStrikeNiagara)
	{
		ThunderStrikeNiagara = InThunderStrikeNiagara;
	}
	
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
	
	if (ThunderStrikeNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ThunderStrikeNiagara,
			StrikeLocation,
			FRotator::ZeroRotator
		);
	}

	ApplyThunderStrikeDamage(StrikeLocation);
	
	CurrentStrikeCount++;
}

void ALThunderActor::ApplyThunderStrikeDamage(const FVector& StrikeLocation)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	/*if (bDrawThunderDamageDebug)
	{
		DrawDebugSphere(
			World,
			StrikeLocation,
			ThunderDamageRadius,
			16,
			FColor::Red,
			false,
			1.0f
		);
	}*/

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
		StrikeLocation,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(ThunderDamageRadius),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	TArray<AActor*> DamagedActors;
	int32 DamagedCount = 0;

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

		const bool bDamageApplied = ApplyDamageToActor(HitActor);

		if (bDamageApplied)
		{
			DamagedCount++;
		}
	}

	ALPlayerCharacter* OwnerPlayer = Cast<ALPlayerCharacter>(GetOwner());

	if (OwnerPlayer && DamagedCount > 0)
	{
		OwnerPlayer->OnSkillHitConfirmed(
			ELPlayerSkillID::Thunder,
			DamagedCount
		);
	}
}

bool ALThunderActor::ApplyDamageToActor(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return false;
	}

	if (!TargetActor->GetClass()->ImplementsInterface(ULDamageable::StaticClass()))
	{
		return false;
	}

	AActor* DamageCauser = GetOwner();

	if (!DamageCauser)
	{
		DamageCauser = this;
	}

	float FinalDamage = ThunderDamage;

	ALPlayerCharacter* OwnerPlayer =
		Cast<ALPlayerCharacter>(DamageCauser);

	if (OwnerPlayer)
	{
		FinalDamage = OwnerPlayer->GetFinalSkillDamage(
			ThunderDamage,
			ELPlayerSkillID::Thunder
		);
	}

	ILDamageable::Execute_ReceiveSkillDamage(
		TargetActor,
		FinalDamage,
		DamageCauser,
		ELPlayerSkillID::Thunder
	);


	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Thunder Damage: %s / Damage: %.1f"),
		*TargetActor->GetName(),
		FinalDamage
	);

	return true;
}
