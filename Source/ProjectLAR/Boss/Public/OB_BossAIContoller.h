// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"
#include "OB_BossAIContoller.generated.h"

UCLASS()
class PROJECTLAR_API AOB_BossAIContoller : public AAIController
{
	GENERATED_BODY()

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
	
	float AcceptanceRadius;
	
	void OnGetPawn(APawn* InPawn);
	void StartMove(AActor* Target);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
