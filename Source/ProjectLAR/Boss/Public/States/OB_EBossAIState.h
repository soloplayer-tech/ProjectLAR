// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "OB_EBossAIState.generated.h"

/**
 * TODO :
 * BOSS의 상태를 열거한다.  
 */

UENUM(BlueprintType)
enum class EBossAIState : uint8
{
	IDLE	UMETA(DisplayName = "Idle"),
	MOVE	UMETA(DisplayName = "Move"),
	ATTACK  UMETA(DisplayName = "Attack"),
	STUNNED UMETA(DisplayName = "Stunned")
};
