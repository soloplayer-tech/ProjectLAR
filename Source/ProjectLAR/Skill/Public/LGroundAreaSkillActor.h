#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LPlayerSkillID.h"
#include "LGroundAreaSkillActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USceneComponent;

UCLASS()
class PROJECTLAR_API ALGroundAreaSkillActor : public AActor
{
	GENERATED_BODY()

public:
	ALGroundAreaSkillActor();

	void InitializeGroundAreaSkill(
		ELPlayerSkillID InSkillID,
		const FVector& InCenterLocation,
		float InDamage,
		float InRadius,
		float InDuration,
		float InTickInterval,
		UNiagaraSystem* InLoopNiagara
	);

protected:
	void ApplyAreaDamage();
	bool ApplyDamageToActor(AActor* TargetActor);
	void EndAreaSkill();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Area|Debug")
	bool bDrawDamageDebug = false;

private:
	ELPlayerSkillID SkillID = ELPlayerSkillID::None;
	float Damage = 0.0f;
	float Radius = 300.0f;
	float Duration = 3.0f;
	float TickInterval = 0.5f;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveLoopNiagara;

	FTimerHandle DamageTickTimerHandle;
	FTimerHandle DurationTimerHandle;
};
