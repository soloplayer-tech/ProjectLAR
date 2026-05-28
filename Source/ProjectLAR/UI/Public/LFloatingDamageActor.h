#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LFloatingDamageActor.generated.h"

class UWidgetComponent;
class ULFloatingDamageWidget;

UCLASS()
class PROJECTLAR_API ALFloatingDamageActor : public AActor
{
	GENERATED_BODY()

public:
	ALFloatingDamageActor();

	virtual void Tick(float DeltaTime) override;

	void InitializeFloatingDamage(float Damage);

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> DamageWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingDamage")
	float LifeTime = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingDamage")
	float FloatSpeed = 80.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingDamage")
	float RandomHorizontalOffset = 20.0f;

private:
	float ElapsedTime = 0.0f;
};