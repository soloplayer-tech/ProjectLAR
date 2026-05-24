// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "OB_BossAIContoller.generated.h"

class UOB_BossFSMComponent;
class AOB_BossCharacter;

UCLASS()
class PROJECTLAR_API AOB_BossAIContoller : public AAIController
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;

public:
	// Sets default values for this actor's properties
	AOB_BossAIContoller();

	/** MoveToActor 변수 설명 인용 
	 *  @brief Makes AI go toward specified Dest location, aborts any active path following
	 *  @param AcceptanceRadius - finish move if pawn gets close enough
	 *  @note AcceptanceRadius has default value or -1 due to Header Parser not being able to recognize UPathFollowingComponent::DefaultAcceptanceRadius
	 */
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="BossControl")
	TObjectPtr<AOB_BossCharacter> BossCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BossControl")
	UOB_BossFSMComponent* FSMComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BossControl")
	float AcceptanceRadius;
	
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComp;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	void StartMove(AActor* Target);
	
	UFUNCTION()
	AActor* GetTargetActor() const { return TargetActor; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void OnPossess(APawn* InPawn) override; // 원본이 protected이기 때문에 protected안에 선언

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
