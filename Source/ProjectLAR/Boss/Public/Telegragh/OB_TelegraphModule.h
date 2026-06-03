#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "OB_LogManager.h"
#include "WorldCollision.h"
#include "Engine/OverlapResult.h" 

// 공격 장판의 형태
enum class ETelegraphType : uint8
{
    Circle, // 원형
    Box,    // 박스형
    Cone
};

class MYPROJECT_API FBossTelegraphModule
{
public:
    DECLARE_DELEGATE(FOnAttackComplete);

    /**
     * @brief [통합형] 원형/박스형 장판을 모두 처리하고 시차 폭발 판정을 내리는 마스터 함수
     * @param World 현재 월드 포인터
     * @param Type 장판 형태 (ETelegraphType::Circle 또는 Box)
     * @param Center 위치 (장판이 깔릴 중심점)
     * @param Size 원형일 때는 X값에 반지름을, 박스형일 때는 X,Y,Z에 절반 크기(Extent)를 입력, 부채꼴일 때 Size.Y는 부채꼴의 총 각도를 입력
     * @param Rotation 박스형일 때 방향과 회전값
     * @param Duration 전조 장판이 유지될 시간 (초)
     * @param OnComplete 공격이 완전히 끝난 후 실행할 함수
     */
    
    static void SpawnTelegraph(
        UWorld* World, 
        ETelegraphType Type,
        const FVector& Center, 
        const FVector& Size, // 원형은 Size.X(반지름) 사용, 박스는 Size(Extent) 통째로 사용
        const FRotator& Rotation,
        float Duration, 
        FOnAttackComplete OnComplete = FOnAttackComplete()
    )
    {
        if (!World) { LOG_TRACE_WARN("World is nullptr!!!"); return; }

        // 타입에 맞춰 빨간색 장판 그리기
        switch (Type)
        {
            case ETelegraphType::Circle:
                LOG_TRACE_INFO(TEXT("Type : Circle, Location : %s, Size : %s, Duration : %f"), Center.ToString(), Size.ToString(), Duration);
                DrawDebugCircle(World, Center, Size.X, 32, FColor::Red, false, Duration, 0, 4.f, FVector(0, 1, 0), FVector(1, 0, 0), false);
                break;

            case ETelegraphType::Box:
                LOG_TRACE_INFO(TEXT("Type : Circle, Location : %s, Size : %s, Rotation : %s, Duration : %f"), Center.ToString(), Size.ToString(), Rotation.ToString() ,Duration);   
                DrawDebugBox(World, Center, Size, Rotation.Quaternion(), FColor::Red, false, Duration, 0, 4.f);
                break;
            case ETelegraphType::Cone:
            
                LOG_TRACE_INFO(TEXT("Type : Circle, Location : %s, Size : %s, Duration : %f"), Center.ToString(), Size.ToString(), Duration);
            
                // Size.X = 반지름, Size.Y = 총 각도
                DrawDebugCone(World, Center, Rotation.Vector(), Size.X, FMath::DegreesToRadians(Size.Y * 0.5f), FMath::DegreesToRadians(Size.Y * 0.5f), 16, FColor::Red, false, Duration, 0, 4.f);
                break;
        }

        // 타이머 설정
        FTimerHandle TelegraphTimer;
        World->GetTimerManager().SetTimer(TelegraphTimer, [World, Type, Center, Size, Rotation, OnComplete]()
        {
            FCollisionShape CollisionShape;

            // 타이머 종료 후 타입에 맞춰 폭발 연출 및 물리 모양 생성
            switch (Type)
            {
                case ETelegraphType::Circle:
                    DrawDebugSphere(World, Center, Size.X, 16, FColor::Green, false, 0.4f, 0, 2.f);
                    CollisionShape = FCollisionShape::MakeSphere(Size.X);
                    break;
                
                case ETelegraphType::Box:
                    DrawDebugBox(World, Center, Size, Rotation.Quaternion(), FColor::Green, false, 0.4f, 0, 2.f);
                    CollisionShape = FCollisionShape::MakeBox(Size);
                    break;
                case ETelegraphType::Cone:
                    DrawDebugCone(World, Center, Rotation.Vector(), Size.X, FMath::DegreesToRadians(Size.Y * 0.5f), FMath::DegreesToRadians(Size.Y * 0.5f), 16, FColor::Green, false, 0.4f, 0, 2.f);
                    CollisionShape = FCollisionShape::MakeSphere(Size.X);
                    break;
            }

            // 실제 오버랩 타격 판정 진행
            TArray<FOverlapResult> OverlapResults;

            bool bHit = World->OverlapMultiByChannel(
                OverlapResults, 
                Center, 
                Type == ETelegraphType::Box ? Rotation.Quaternion() : FQuat::Identity, // 박스일 때만 회전값 적용
                ECC_Pawn, 
                CollisionShape
            );

            if (bHit)
            {
                for (const FOverlapResult& Result : OverlapResults)
                {
                    AActor* HitActor = Result.GetActor();
                    if (HitActor && HitActor->ActorHasTag(FName("Player")))
                    {
                        if (Type == ETelegraphType::Cone)
                        {
                            FVector ForwardVector = Rotation.Vector().GetSafeNormal2D(); // 보스의 정면 방향
                            FVector TargetVector = (HitActor->GetActorLocation() - Center).GetSafeNormal2D(); // 보스 -> 플레이어 방향

                            // 두 벡터의 내적(Dot Product)을 통해 사이각을 구합니다.
                            float DotProduct = FVector::DotProduct(ForwardVector, TargetVector);
                            float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

                            // 사이각이 절반 각도(120도의 절반인 60도)보다 크다면 부채꼴 범위를 벗어난 것이므로 패스!
                            float HalfConeAngle = Size.Y * 0.5f;
                            if (AngleToTarget > HalfConeAngle)
                            {
                                continue; 
                            }
                        }
                        
                        LOG_TRACE_INFO(TEXT("[Integrated Telegraph] Player Hit Success!"));
                    }
                }
            }

            // 공격 종료 후 FSM 상태 복귀 통보
            if (OnComplete.IsBound()) OnComplete.Execute();

        }, Duration, false);
    }
};