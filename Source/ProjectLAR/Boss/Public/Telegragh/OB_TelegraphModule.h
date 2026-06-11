#pragma once

#include "Engine/World.h"
#include "TimerManager.h"
#include "LPlayerCharacter.h"
#include "OB_LogManager.h"
#include "WorldCollision.h"
#include "Engine/OverlapResult.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

// 공격 장판의 형태
enum class ETelegraphType : uint8
{
    Circle, // 원형
    Box,    // 박스형
    Cone
};

class FBossTelegraphModule
{
public:
    DECLARE_DELEGATE_OneParam(FOnAttackComplete, bool bHit);

    /**
     * @brief [통합형] 원형/박스형 장판을 모두 처리하고 시차 폭발 판정을 내리는 마스터 함수
     * @param World 현재 월드 포인터
     * @param DecalMaterial 데칼(장판)에 사용되는 Material
     * @param Type 장판 형태 (ETelegraphType::Circle 또는 Box)
     * @param Center 위치 (장판이 깔릴 중심점)
     * @param Size 원형일 때는 X값에 반지름을, 박스형일 때는 X,Y,Z에 절반 크기(Extent)를 입력, 부채꼴일 때 Size.Y는 부채꼴의 총 각도를 입력
     * @param Rotation 박스형일 때 방향과 회전값
     * @param Duration 전조 장판이 유지될 시간 (초)
     * @param OnComplete 공격이 완전히 끝난 후 실행할 함수
     */
    
    static void SpawnTelegraph(
        UWorld* World, 
        UMaterialInterface* DecalMaterial, // 데칼용 머테리얼
        ETelegraphType Type,
        const FVector& Center, 
        const FVector& Size, // 원형은 Size.X(반지름) 사용, 박스는 Size(Extent) 통째로 사용
        const FRotator& Rotation,
        float Duration, 
        FOnAttackComplete OnComplete = FOnAttackComplete()
    )
    {
        if (!World || !DecalMaterial) { LOG_TRACE_WARN("World is nullptr!!!"); return; }
        
        // Box는 Size를 전부 사용하고 원형 (Circle & Cone)은 Size.X만 사용한다.
        FVector DecalSize;
        
        if (Type == ETelegraphType::Box)
        {
            DecalSize = FVector(Size.Z,Size.Y,Size.X);
        }
        else
        {
            DecalSize = FVector(Size.X,Size.X,Size.X);
        }
        
        FRotator SpawnRotation = Rotation+FRotator(-90.f, 0.f, 0.f);

        UDecalComponent* DecalComp = UGameplayStatics::SpawnDecalAtLocation(
            World, DecalMaterial, DecalSize, Center, SpawnRotation, Duration + 0.5f
        ); // 게임 세계에 데칼을 DecalSize 크기로 중심에 생성시 각도를 SpawnRotation 해서 Duration + 0.5f 초 동안
        
        UMaterialInstanceDynamic* DynamicMaterial = nullptr;
        
        if (DecalComp)
        {
            DynamicMaterial = DecalComp->CreateDynamicMaterialInstance();
            LOG_TRACE_WARN(TEXT("DynamicMaterial : %s"), DynamicMaterial ? TEXT("Valid") : TEXT("NULL"));
            
            if (DynamicMaterial)
            {
                DecalComp->SetMaterial(0, DynamicMaterial);  // ← 추가
                DynamicMaterial->SetScalarParameterValue(TEXT("Progress"), 0.f);  // ← 초기값
        
                if (Type == ETelegraphType::Cone)
                    DynamicMaterial->SetScalarParameterValue(TEXT("ConeAngle"), Size.Y / 360.f);
            }
        }

        TSharedPtr<FTimerHandle> TimerHandlePtr = MakeShared<FTimerHandle>();
        TSharedPtr<float> ElapsedTimePtr = MakeShared<float>(0.f);
        float UpdateInterval = 0.033f;

        World->GetTimerManager().SetTimer(*TimerHandlePtr, [World, Type, Center, Size, Rotation, Duration, DecalComp, DynamicMaterial, OnComplete, TimerHandlePtr, ElapsedTimePtr, UpdateInterval]()
        {
            if (!World) return;

            *ElapsedTimePtr += UpdateInterval;
            float Alpha = FMath::Clamp(*ElapsedTimePtr / Duration, 0.f, 1.f);
            
            if (DynamicMaterial)
            {
                DynamicMaterial->SetScalarParameterValue(TEXT("Progress"), Alpha);
            }

            
            if (*ElapsedTimePtr >= Duration)
            {
                World->GetTimerManager().ClearTimer(*TimerHandlePtr);
                if (DecalComp) DecalComp->DestroyComponent(); // 데칼 제거

                FCollisionShape CollisionShape;
                
                switch (Type)
                {
                    case ETelegraphType::Circle:   
                        LOG_TRACE_INFO(TEXT("Type : Circle, Location : %s, Size : %s, Duration : %f"), *Center.ToString(), *Size.ToString(), Duration);
                        CollisionShape = FCollisionShape::MakeSphere(Size.X);  break;
                    case ETelegraphType::Box:    
                        LOG_TRACE_INFO(TEXT("Type : Box, Location : %s, Size : %s, Rotation : %s, Duration : %f"), *Center.ToString(), *Size.ToString(), *Rotation.ToString() ,Duration);   
                        CollisionShape = FCollisionShape::MakeBox(Size);       break;
                    case ETelegraphType::Cone:   
                        LOG_TRACE_INFO(TEXT("Type : Cone, Location : %s, Size : %s, Duration : %f"), *Center.ToString(), *Size.ToString(), Duration);
                        CollisionShape = FCollisionShape::MakeSphere(Size.X);  break;
                }

                TArray<FOverlapResult> OverlapResults;
                bool bOverlapAny = World->OverlapMultiByChannel(
                    OverlapResults, Center, Type == ETelegraphType::Box ? Rotation.Quaternion() : FQuat::Identity, ECC_Pawn, CollisionShape
                );

                bool bRealPlayerHit = false;

                if (bOverlapAny)
                {
                    for (const FOverlapResult& Result : OverlapResults)
                    {
                        AActor* HitActor = Result.GetActor();
                        if (Cast<ALPlayerCharacter>(HitActor))
                        {
                            if (Type == ETelegraphType::Cone)
                            {
                                FVector ForwardVector = Rotation.Vector().GetSafeNormal2D();
                                FVector TargetVector = (HitActor->GetActorLocation() - Center).GetSafeNormal2D();
                                float DotProduct = FVector::DotProduct(ForwardVector, TargetVector);
                                float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

                                float HalfConeAngle = Size.Y * 0.5f;
                                if (AngleToTarget > HalfConeAngle) continue; 
                            }
                            
                            bRealPlayerHit = true;
                            LOG_TRACE_INFO(TEXT("[Integrated Telegraph] Player Hit Success!"));
                            break; 
                        }
                    }
                }

                // 컴뱃 쪽에 플레이어 타격 결과를 포함해 통보
                if (OnComplete.IsBound()) OnComplete.Execute(bRealPlayerHit);
            }
        }, UpdateInterval, true);
    }
};