// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "OB_EBossBattleState.generated.h"

/**  
 *  
 */

UENUM(BlueprintType)
enum class EBossBattleState : uint8
{
	IDLE	UMETA(DisplayName = "Idle"),
	MOVE	UMETA(DisplayName = "Move"),
	ATTACK  UMETA(DisplayName = "Attack"),
	STUNNED UMETA(DisplayName = "Stunned")
};
