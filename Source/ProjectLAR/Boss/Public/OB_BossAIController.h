// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "OB_BossAIController.generated.h"

class UNavigationSystemV1;
class UOB_BossFSMComponent;
class AOB_BossCharacter;

UCLASS()
class PROJECTLAR_API AOB_BossAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOB_BossAIController();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Boss|Sight")
	double Sight_Radius = 1000.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Boss|Sight")
	float MinRadius = 300.f;  
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Boss|Sight")
	float MaxRadius = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BossControl")
	float AcceptanceRadius = 200.f;
	
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	void StartMove();
	void OnFindTarget(AActor* TargetActor);
	void TeleportRandomlyAroundTarget(const FVector& TargetLocation);
	void SetBossLocation(const FVector& TargetLocation, const FNavLocation& RandomNavLocation);
	bool FindSafetyLocation(const FVector& TargetLocation, FNavLocation& SafetyLocation);

	/** MoveToActor 변수 설명 인용 
	 *  @brief Makes AI go toward specified Dest location, aborts any active path following
	 *  @param AcceptanceRadius - finish move if pawn gets close enough
	 *  @note AcceptanceRadius has default value or -1 due to Header Parser not being able to recognize UPathFollowingComponent::DefaultAcceptanceRadius
	 */

private:
	
	UPROPERTY()
	TObjectPtr<AOB_BossCharacter> BossCharacter;
	
	UPROPERTY()
	UOB_BossFSMComponent* FSMComp;
	
	UPROPERTY()
	UAIPerceptionComponent* PerceptionComp;
	
	UPROPERTY()
	UWorld* World;
	
	UPROPERTY()
	UNavigationSystemV1* NavSystem;
	
	// void OnMoveCompleted(FAIRequestID RequestID,  EPathFollowingResult::Type Result) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override; // 원본이 protected이기 때문에 protected안에 선언

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
