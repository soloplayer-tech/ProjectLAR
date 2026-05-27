// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LDamageable.h"
#include "GameFramework/Actor.h"
#include "LTestDamageBox.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECTLAR_API ALTestDamageBox : public AActor, public ILDamageable
{
	GENERATED_BODY()
	
public:
	ALTestDamageBox();
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void ReceiveSkillDamage_Implementation(
	float Damage,
	AActor* DamageCauser,
	ELPlayerSkillID SkillID
	) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UBoxComponent> HitBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxHP = 10000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float CurHP = 10000.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bDestroyOnDeath = true;
	
private:
	void Die();
};