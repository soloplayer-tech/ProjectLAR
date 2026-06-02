// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OB_EAttackPattern.generated.h"

/**
 *  @brief Boss의 공격패턴을 열거한다.
 */
UENUM(BlueprintType)
enum class EAttackPattern : uint8
{
	HAMMER	UMETA(DisplayName = "Hammer Pattern"),
	RUSH	UMETA(DisplayName = "Rush Pattern"),
	SLAM	UMETA(DisplayName = "Slam Pattern"),
	CARD	UMETA(DisplayName = "Card Pattern"),
};