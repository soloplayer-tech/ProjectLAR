// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectLAR/Combat/Public/LDamageable.h"
#include "OB_CombatComponent.generated.h"


/**
 * TODO : 
 * - [ ] : Distance 구하기 
 * - [ ] : Slam 기능 구현
 * - [ ] : Hammer 기능 구현
 * - [ ] : Rush 기능 구현
 * - [ ] : Card 기능 구현
 * - [ ] : CharacterHP 받아서 State 분기 처리 -- 현재는 일단 랜덤 값으로 계산
 */

class ALFloatingDamageActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTLAR_API UOB_CombatComponent : public UActorComponent, public ILDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOB_CombatComponent();

	void StartAttack();
	void StopAttack();
	
	bool bDestroyOnDeath = true;
	
	void TakeDamage(float DamageAmount);
	
	bool IsDead() const { return CurHP <= 0.f; }
	
	float GetCurHP() const { return CurHP; }
	float GetHPRatio() const { return CurHP / MaxHP; }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ALFloatingDamageActor> FloatingDamageActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	float FloatingDamageHeightOffset = 140.0f;
	
private:     
	
	// 캐싱
	UPROPERTY()
	class AOB_BossCharacter* OwnerCharacter;

	UPROPERTY()
	class UOB_PatternComponent* PatternComp;

	 // HP
    UPROPERTY(EditDefaultsOnly, Category="Combat|HP")
    float MaxHP = 1000.f;

    UPROPERTY(VisibleAnywhere, Category="Combat|HP")
    float CurHP;

    // 쿨타임
    UPROPERTY(EditDefaultsOnly, Category="Combat|Attack")
    float AttackCooldown = 2.0f;

    FTimerHandle CooldownTimer;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Attack")
    bool bCanAttack = true;

    void OnCooldownFinished();
    void OnDead();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
