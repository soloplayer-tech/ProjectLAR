#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LIceLanceActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTLAR_API ALIceLanceActor : public AActor
{
	GENERATED_BODY()
	
public:
	ALIceLanceActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	void InitializeBezierPath(
		const FVector& InStartPoint,
		const FVector& InControlPoint,
		const FVector& InEndPoint,
		float InTravelDuration,
		float InFireDelay
	);

protected:
	// 2차 베지어 곡선 위치 계산
	FVector GetQuadraticBezierPoint(float T) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> IceLanceMesh;

private:
	FVector StartPoint;
	FVector ControlPoint;
	FVector EndPoint;

	// 날아가는 데 걸리는 시간
	float TravelDuration = 0.6f;

	// 비행 시작 후 경과 시간
	float ElapsedTime = 0.0f;

	// 생성 후 발사되기 전까지 기다릴 시간
	float FireDelay = 0.0f;

	// 현재 대기 경과 시간
	float WaitElapsedTime = 0.0f;

	// 경로 정보가 들어왔는가
	bool bPathInitialized = false;

	// 지금 실제로 날아가는 중인가?
	bool bIsFlying = false;
};