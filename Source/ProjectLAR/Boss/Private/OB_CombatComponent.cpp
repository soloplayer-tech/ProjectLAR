// Fill out your copyright notice in the Description page of Project Settings.


#include "OB_CombatComponent.h"
#include "OB_BossCharacter.h"
#include "OB_BossFSMComponent.h"


// Sets default values for this component's properties
UOB_CombatComponent::UOB_CombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	CurHP = MaxHP;
	
	OwnerCharacter = Cast<AOB_BossCharacter>(GetOwner());
	if (OwnerCharacter != nullptr)
	{
		PatternComp = OwnerCharacter -> GetPatternComponent();
	}
}

void UOB_CombatComponent::StartAttack()
{
	LOG_TRACE_INFO("Call StartAttack")
	
	if (!bCanAttack)
	{
		LOG_TRACE_INFO("Is CoolTime");
		return;
	}
	
	if (!PatternComp)
	{
		LOG_TRACE_WARN("PatternComp is null!!");
		return;
	}
	
	PatternComp -> SelectAndExecute();
	
	bCanAttack = false;
	GetWorld() -> GetTimerManager().SetTimer(
		CooldownTimer,
		this,
		&UOB_CombatComponent::OnCooldownFinished,
		AttackCooldown,
		false
	);
	
}

void UOB_CombatComponent::StopAttack()
{
	LOG_TRACE_INFO("Call StopAttack");
	
	GetWorld() -> GetTimerManager().ClearTimer(CooldownTimer);
	bCanAttack = true;
}

void UOB_CombatComponent::TakeDamage(float DamageAmount)
{
	if (IsDead()) return;
	
	CurHP = FMath::Clamp( CurHP - DamageAmount , 0.f, MaxHP );
	
	LOG_TRACE_INFO(TEXT("[Combat] HP : %.0f / %.0f (%.0f%%)"), CurHP, MaxHP, GetHPRatio() * 100.0f);
	
	// TODO : Phase2 진입시 ( HP <= 50% )
	
	if (IsDead()) OnDead();
}


void UOB_CombatComponent::OnCooldownFinished()
{
	bCanAttack = true;
	LOG_TRACE_INFO("CoolTime is Finished");
	
	if ( OwnerCharacter )
	{
		auto* FSMComp = OwnerCharacter -> GetFSMComponent();
		if ( FSMComp && FSMComp -> GetCurState()==EBossBattleState::ATTACK)
		{
			PatternComp -> SelectAndExecute();
		}
	}
}

void UOB_CombatComponent::OnDead()
{
	LOG_TRACE_WARN("[Combat] Boss is Dead");
	
	if ( OwnerCharacter )
	{
		auto* FSMComp = OwnerCharacter->GetFSMComponent();
		
		if (FSMComp)
			FSMComp->SetState(EBossBattleState::DEAD);
	}
	
}

// Called when the game starts
void UOB_CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOB_CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

