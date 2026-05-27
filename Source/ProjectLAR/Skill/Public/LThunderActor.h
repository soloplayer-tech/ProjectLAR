
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LThunderActor.generated.h"

class UNiagaraSystem;
class USceneComponent;

UCLASS()
class PROJECTLAR_API ALThunderActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALThunderActor();
	
public:
	void InitializeThunderStorm(const FVector& InCenterLocation);
	
private:
	void SpawnThunderStrike();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Component")
	TObjectPtr<USceneComponent> RootScene;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Thunder")
	TObjectPtr<UNiagaraSystem> ThunderStrikeNiagara;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Thunder")
	float StrikeRadius = 360.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Thunder")
	int32 StrikeCount = 6;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Thunder")
	float StrikeInterval = 0.12f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Thunder")
	float StrikeHeightOffset = 0.f;
	
private:
	FVector CenterLocation;
	
	int32 CurrentStrikeCount = 0;
	
	FTimerHandle StrikeTimerHandle;
};
