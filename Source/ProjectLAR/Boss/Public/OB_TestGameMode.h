// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OB_TestGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTestGameMode, Log, All);

class ATargetPoint;
/**
 * 
 */
UCLASS()
class PROJECTLAR_API AOB_TestGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Level Boss");
	TObjectPtr<ATargetPoint> BossSpawnPoint;
	
	UPROPERTY(EditAnywhere, Category="Level Boss");
	TSubclassOf<ACharacter> BossClass;;

	void SpawnBoss();
	void SpawnBossAtRandLocation(FVector, float);
};
