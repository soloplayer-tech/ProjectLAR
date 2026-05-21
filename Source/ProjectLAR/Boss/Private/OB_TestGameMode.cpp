// Fill out your copyright notice in the Description page of Project Settings.

// TODO: 의존 확인해보기

#include "OB_TestGameMode.h"

#include "NavigationSystem.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY( LogTestGameMode );

void AOB_TestGameMode::SpawnBoss()
{
	UE_LOG( LogTestGameMode, Log, TEXT("SpawnBoss"));
	// ensureMsgf 배치 여부 로그처리
	if (ensureMsgf(BossSpawnPoint != nullptr, TEXT("BossSpawnPoint is nullptr")))
	{
		FVector SpawnLocation = BossSpawnPoint-> GetActorLocation();
		FRotator SpawnRotation = BossSpawnPoint -> GetActorRotation();
		
		GetWorld() -> SpawnActor<ACharacter>(BossClass, SpawnLocation, SpawnRotation);
	}
}

void AOB_TestGameMode::SpawnBossAtRandLocation(FVector OriginLocation, float Radius)
{
	UE_LOG( LogTestGameMode, Log, TEXT("SpawnBossAtRandLocation"));
	
	// NavSystem 설정
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (ensureMsgf(NavSys != nullptr, TEXT("There is no Navigation System, Check the NavMeshBoundVolume")))
	{
		FNavLocation RandomLocation;
		// OriginLocation(예: 플레이어 위치) 주변 반지름 Radius 영역 내의 걸어다닐 수 있는 바닥 검색
		if (NavSys->GetRandomPointInNavigableRadius(OriginLocation, Radius, RandomLocation))
		{
			GetWorld()->SpawnActor<ACharacter>(BossClass, RandomLocation.Location, FRotator::ZeroRotator);
		}
	}
}
