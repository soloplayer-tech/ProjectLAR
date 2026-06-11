// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_PatternComponent.h"

#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
#include "OB_LogManager.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/mat.hpp"
#include "Telegragh/OB_TelegraphModule.h"


// Sets default values for this component's properties
UOB_PatternComponent::UOB_PatternComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	// ...
	Mat_Circle = nullptr;
	Mat_Box = nullptr;
	Mat_Cone = nullptr;
}


// Called when the game starts
void UOB_PatternComponent::BeginPlay()
{
	Super::BeginPlay();
	
	LOG_TRACE_INFO();
	// ...
	OwnerCharacter = Cast<AOB_BossCharacter>(GetOwner()); 
	
}


// Called every frame
void UOB_PatternComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UOB_PatternComponent::SelectAndExecute()
{
	LOG_TRACE_INFO();
	CurPattern = SelectPattern();
	ExecutePattern(CurPattern);
}

void UOB_PatternComponent::SetPhase(int32 Phase)
{
	// TODO : 페이즈 구분을 둘 때 작업 시작 
}

void UOB_PatternComponent::OnPatternFinished()
{
	LOG_TRACE_INFO("Pattern is Finished");
	
	if ( OwnerCharacter )
	{
		auto* FSMComp = OwnerCharacter -> GetFSMComponent();
		if ( FSMComp )
		{
			FSMComp -> SetState(EBossBattleState::MOVE); 
		}
	}
}

EAttackPattern UOB_PatternComponent::SelectPattern()
{
	// TODO : 나중에 여기서 거리/HP 조건 추가 
	TArray AvailablePatterns = {
		EAttackPattern::AXE,
		EAttackPattern::RUSH
	};

	int32 index = FMath::RandRange(0, AvailablePatterns.Num() - 1);
	return AvailablePatterns[index];
}

void UOB_PatternComponent::ExecutePattern(EAttackPattern Pattern)
{
	LOG_TRACE_INFO(TEXT("[ Pattern : %s ]"),*UEnum::GetValueAsString(Pattern));
	
	switch (Pattern)
	{
	case EAttackPattern::AXE:	ExecuteAxe();	break;
	case EAttackPattern::RUSH:		ExecuteRush();		break;
	case EAttackPattern::SLAM:		ExecuteSlam();		break;
	case EAttackPattern::CARD:		ExecuteCard();		break;
	}	
}

void UOB_PatternComponent::ExecuteAxe()
{
	LOG_TRACE_INFO("[Pattern Axe]");
	
	if (!Mat_Cone) 
	{
		LOG_TRACE_WARN("Telegraph Material is not assigned!");
		return;
	}
	
	FVector BossLocation = OwnerCharacter -> GetActorLocation();
	FVector TargetLocation = OwnerCharacter -> GetFSMComponent() ->GetTargetActor() ->GetActorLocation();
	
	FVector LookDirection = TargetLocation - BossLocation;
	
	FVector CorrectForward = LookDirection.GetSafeNormal2D();
	
	FRotator NewRotation = CorrectForward.Rotation();
	NewRotation.Pitch = -30.f; 
	NewRotation.Roll = 0.f;
	
	OwnerCharacter -> SetActorRotation(NewRotation);
	
	FVector ConeExtent = FVector(500.f, 240.f, 10.f);
	
	
	FBossTelegraphModule::SpawnTelegraph(
		GetWorld(),
		Mat_Cone,
		ETelegraphType::Cone,
		BossLocation,
		ConeExtent,
		NewRotation,
		1.0f,
		FBossTelegraphModule::FOnAttackComplete::CreateLambda([this](bool bHit)
		{
			if (OwnerCharacter -> GetFSMComponent()) OwnerCharacter -> GetFSMComponent() -> SetState(EBossBattleState::MOVE);
		})
	);
	
}

void UOB_PatternComponent::ExecuteRush()
{
	LOG_TRACE_INFO("[Pattern Rush]");
	
	if (!Mat_Box) 
	{
		LOG_TRACE_WARN("Telegraph Material is not assigned!");
		return;
	}
	
	FVector BossLocation = OwnerCharacter -> GetActorLocation();
	FVector TargetLocation = OwnerCharacter -> GetFSMComponent() -> GetTargetActor() ->GetActorLocation();
	
	FVector LookDirection = TargetLocation - BossLocation;
	
	FVector CorrectForward = LookDirection.GetSafeNormal2D();
	
	FRotator NewRotation = CorrectForward.Rotation();
	NewRotation.Pitch = 0.f; 
	NewRotation.Roll = 0.f;
	
	OwnerCharacter -> SetActorRotation(NewRotation);
	
	FVector BoxExtent = FVector(500.f, 100.f, 300.f);
	FVector AdjustedCenter = BossLocation + (CorrectForward * BoxExtent.X);
	
	FBossTelegraphModule::SpawnTelegraph(
		GetWorld(),
		Mat_Box,
		ETelegraphType::Box,
		AdjustedCenter,
		BoxExtent,
		NewRotation,
		1.0f,
		FBossTelegraphModule::FOnAttackComplete::CreateLambda([this](bool bHitBox)
		{
			ExecuteSlam();
		})
		
	);
}

void UOB_PatternComponent::ExecuteSlam()
{
	LOG_TRACE_INFO("[Pattern Slam]");
	
	if (!Mat_Circle) 
	{
		LOG_TRACE_WARN("Telegraph Material is not assigned!");
		return;
	}
	
	FVector BossLocation = OwnerCharacter -> GetActorLocation();
	FVector TargetLocation = OwnerCharacter -> GetFSMComponent() ->GetTargetActor() ->GetActorLocation();
	
	FVector LookDirection = TargetLocation - BossLocation;
	
	FVector CorrectForward = LookDirection.GetSafeNormal2D();
	
	FRotator NewRotation = CorrectForward.Rotation();
	NewRotation.Pitch = 0.f; 
	NewRotation.Roll = 0.f;
	
	OwnerCharacter -> SetActorRotation(NewRotation);
	
	FVector CircleExtent = FVector(1500.f, 120.f, 10.f);
	
	FBossTelegraphModule::SpawnTelegraph(
		GetWorld(),
		Mat_Circle,
		ETelegraphType::Circle,
		BossLocation + FVector(1000.f, 0.f, 0.f),
		CircleExtent,
		NewRotation,
		1.0f,
		FBossTelegraphModule::FOnAttackComplete::CreateLambda([this](bool bHit)
		{
			if (OwnerCharacter -> GetFSMComponent()) OwnerCharacter -> GetFSMComponent() -> SetState(EBossBattleState::MOVE);
		})
	);
}

void UOB_PatternComponent::ExecuteCard()
{
	LOG_TRACE_INFO("[Pattern Card]");
	
	if (!Mat_Circle) 
	{
		LOG_TRACE_WARN("Telegraph Material is not assigned!");
		return;
	}
	
	FVector BossLocation = OwnerCharacter -> GetActorLocation();
	FVector TargetLocation = OwnerCharacter -> GetFSMComponent() ->GetTargetActor() ->GetActorLocation();
	
	FVector LookDirection = TargetLocation - BossLocation;
	
	FVector CorrectForward = LookDirection.GetSafeNormal2D();
	
	FRotator NewRotation = CorrectForward.Rotation();
	NewRotation.Pitch = 0.f; 
	NewRotation.Roll = 0.f;
	
	OwnerCharacter -> SetActorRotation(NewRotation);
	
	FVector CircleExtent = FVector(1500.f, 120.f, 10.f);
	
	FBossTelegraphModule::SpawnTelegraph(
		GetWorld(),
		Mat_Circle,
		ETelegraphType::Circle,
		BossLocation + FVector(1000.f, 0.f, 0.f),
		CircleExtent,
		NewRotation,
		1.0f,
		FBossTelegraphModule::FOnAttackComplete::CreateLambda([this](bool bHit)
		{
			if (OwnerCharacter -> GetFSMComponent()) OwnerCharacter -> GetFSMComponent() -> SetState(EBossBattleState::MOVE);
		})
	);
}


