#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LIceLanceActor.generated.h"

class UStaticMeshComponent;
class UNiagaraSystem;

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

	// 착탄 순간 범위 데미지
	void ApplyImpactDamage();

	// 실제 데미지 적용
	bool ApplyDamageToActor(AActor* TargetActor);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> IceLanceMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float IceLanceDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float ImpactDamageRadius = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDrawImpactDebug = false;

private:
	FVector StartPoint;
	FVector ControlPoint;
	FVector EndPoint;

	float TravelDuration = 0.6f;
	float ElapsedTime = 0.0f;

	float FireDelay = 0.0f;
	float WaitElapsedTime = 0.0f;

	bool bPathInitialized = false;
	bool bIsFlying = false;

	// 착탄 데미지가 여러 번 들어가지 않게 막기
	bool bImpactDamageApplied = false;
};