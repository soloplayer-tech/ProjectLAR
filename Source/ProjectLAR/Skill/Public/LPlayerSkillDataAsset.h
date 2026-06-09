#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LPlayerSkillID.h"
#include "LPlayerSkillTuning.h"
#include "LPlayerSkillDataAsset.generated.h"

class AActor;
class UAnimMontage;
class UNiagaraSystem;
class UTexture2D;

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLPlayerSkillCombatTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat")
	bool bOverrideCombatValues = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat", meta = (ClampMin = "0"))
	int32 Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float Radius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float SpawnRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat", meta = (ClampMin = "0.01"))
	float TickInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat")
	float SpawnHeightOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat")
	float ForwardOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat")
	float HeightOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat")
	float BoxCenterOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat")
	float BoxHeightOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Combat")
	FVector BoxHalfExtent = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLPlayerSkillIdentityTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Identity")
	bool bOverrideIdentityGain = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Identity", meta = (ClampMin = "0.0"))
	float BaseGain = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Identity", meta = (ClampMin = "0.0"))
	float AdditionalGainPerTarget = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Identity", meta = (ClampMin = "0.0"))
	float MaxGainPerEvent = 0.0f;
};

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLPlayerSkillVisualTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Visual")
	TObjectPtr<UNiagaraSystem> MainNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Visual")
	TObjectPtr<UNiagaraSystem> CastStartNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Visual")
	TObjectPtr<UNiagaraSystem> WarningNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Visual")
	TObjectPtr<UNiagaraSystem> ImpactNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Visual")
	float CastStartHeightOffset = 0.0f;
};

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLPlayerIceLanceTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	bool bOverrideIceLanceValues = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance", meta = (ClampMin = "1"))
	int32 Count = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance", meta = (ClampMin = "0.01"))
	float TravelDuration = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	float ReadyBackOffset = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	float ReadySideSpacing = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	float ReadyHeight = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	float ReadyHeightFalloff = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	float CurveSideOffset = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	float CurveHeightOffset = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance")
	float EndHeightOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance", meta = (ClampMin = "0.0"))
	float ReadyDuration = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|IceLance", meta = (ClampMin = "0.0"))
	float FireInterval = 0.05f;
};

UCLASS(BlueprintType)
class PROJECTLAR_API ULPlayerSkillDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	ELPlayerSkillID SkillID = ELPlayerSkillID::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|UI")
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Execution")
	TSubclassOf<AActor> SkillActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FLPlayerSkillTuning Tuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Combat")
	FLPlayerSkillCombatTuning CombatTuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	FLPlayerSkillIdentityTuning IdentityTuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Visual")
	FLPlayerSkillVisualTuning VisualTuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|IceLance")
	FLPlayerIceLanceTuning IceLanceTuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> SkillMontage;
};
