// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "States/OB_EAttackPattern.h"
#include "OB_PatternComponent.generated.h"


class AOB_BossCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTLAR_API UOB_PatternComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOB_PatternComponent();
	
	void SelectAndExecute();          // CombatComp가 호출
	void SetPhase(int32 Phase);       // CombatComp가 Phase 알려줌
	void OnPatternFinished();

private:
	static EAttackPattern SelectPattern();
	
	void ExecutePattern(EAttackPattern Pattern);  
	
	void ExecuteHammer();
	void ExecuteRush();
	void ExecuteSlam();
	void ExecuteCard();

	int32 CurPhase = 1; 
	EAttackPattern CurPattern;
	
	TObjectPtr<AOB_BossCharacter> OwnerCharacter;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
