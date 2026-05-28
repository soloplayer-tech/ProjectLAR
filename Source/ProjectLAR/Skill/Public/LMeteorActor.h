// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMeteorActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;

UCLASS()
class PROJECTLAR_API ALMeteorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALMeteorActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	void InitializeMeteor(const FVector& InImpactLocation);
	
private:
	void Impact();
	
protected:

	// 착탄 순간 범위 데미지
	void ApplyImpactDamage();

	// 실제 데미지 적용
	void ApplyDamageToActor(AActor* TargetActor);
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> MeteorNiagaraComp;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Meteor")
	TObjectPtr<UNiagaraSystem> MeteorWarningNiagara;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Meteor")
	TObjectPtr<UNiagaraSystem> ImpactNiagara;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Meteor")
	float MeteorStartHeight = 1200.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Meteor")
	float FallDuration = 1.f;
	
	// =========================
	// Damage
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|Damage")
	float MeteorDamage = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|Damage")
	float ImpactDamageRadius = 220.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor|Debug")
	bool bDrawImpactDebug = true;
	
private:
	FVector StartLocation;
	FVector ImpactLocation;
	
	float ElapsedTime = 0.f;
	bool bInitialized = false;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> WarningNiagaraComp;
	
	bool bImpactHandled = false;
};
