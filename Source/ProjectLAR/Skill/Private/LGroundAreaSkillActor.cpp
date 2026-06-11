#include "LGroundAreaSkillActor.h"

#include "DrawDebugHelpers.h"
#include "LPlayerCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"

ALGroundAreaSkillActor::ALGroundAreaSkillActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);
}

void ALGroundAreaSkillActor::InitializeGroundAreaSkill(
	ELPlayerSkillID InSkillID,
	const FVector& InCenterLocation,
	float InDamage,
	float InRadius,
	float InDuration,
	float InTickInterval,
	UNiagaraSystem* InLoopNiagara
)
{
	SkillID = InSkillID;
	Damage = FMath::Max(0.0f, InDamage);
	Radius = FMath::Max(1.0f, InRadius);
	Duration = FMath::Max(0.01f, InDuration);
	TickInterval = FMath::Max(0.05f, InTickInterval);

	SetActorLocation(InCenterLocation);

	if (InLoopNiagara)
	{
		ActiveLoopNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(
			InLoopNiagara,
			RootScene,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}

	if (ALPlayerCharacter* OwnerPlayer = Cast<ALPlayerCharacter>(GetOwner()))
	{
		OwnerPlayer->PlaySkillImpactSound(SkillID, InCenterLocation);
	}

	ApplyAreaDamage();

	GetWorldTimerManager().ClearTimer(DamageTickTimerHandle);
	GetWorldTimerManager().SetTimer(
		DamageTickTimerHandle,
		this,
		&ALGroundAreaSkillActor::ApplyAreaDamage,
		TickInterval,
		true,
		TickInterval
	);

	GetWorldTimerManager().ClearTimer(DurationTimerHandle);
	GetWorldTimerManager().SetTimer(
		DurationTimerHandle,
		this,
		&ALGroundAreaSkillActor::EndAreaSkill,
		Duration,
		false
	);
}

void ALGroundAreaSkillActor::ApplyAreaDamage()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	if (bDrawDamageDebug)
	{
		DrawDebugSphere(
			World,
			GetActorLocation(),
			Radius,
			24,
			FColor::Cyan,
			false,
			TickInterval
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
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
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

		if (!HitActor || DamagedActors.Contains(HitActor))
		{
			continue;
		}

		DamagedActors.Add(HitActor);

		if (ApplyDamageToActor(HitActor))
		{
			DamagedCount++;
		}
	}

	ALPlayerCharacter* OwnerPlayer = Cast<ALPlayerCharacter>(GetOwner());

	if (OwnerPlayer && DamagedCount > 0)
	{
		OwnerPlayer->OnSkillHitConfirmed(SkillID, DamagedCount);
	}
}

bool ALGroundAreaSkillActor::ApplyDamageToActor(AActor* TargetActor)
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

	float FinalDamage = Damage;

	if (ALPlayerCharacter* OwnerPlayer = Cast<ALPlayerCharacter>(DamageCauser))
	{
		FinalDamage = OwnerPlayer->GetFinalSkillDamage(Damage, SkillID);
	}

	ILDamageable::Execute_ReceiveSkillDamage(
		TargetActor,
		FinalDamage,
		DamageCauser,
		SkillID
	);

	return true;
}

void ALGroundAreaSkillActor::EndAreaSkill()
{
	GetWorldTimerManager().ClearTimer(DamageTickTimerHandle);
	GetWorldTimerManager().ClearTimer(DurationTimerHandle);

	if (ActiveLoopNiagara)
	{
		ActiveLoopNiagara->Deactivate();
		ActiveLoopNiagara = nullptr;
	}

	Destroy();
}
