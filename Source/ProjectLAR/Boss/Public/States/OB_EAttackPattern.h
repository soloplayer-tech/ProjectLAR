// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "OB_EAttackPattern.generated.h"

/**
 * TODO:
 * Boss의 공격패턴을 열거한다.
 */
UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	HAMMER	UMETA(DisplayName = "Hammer Pattern"),
	RUSH	UMETA(DisplayName = "Rush Pattern"),
	CARD	UMETA(DisplayName = "Card Pattern"),
	SLAM	UMETA(DisplayName = "Slam Pattern")
};