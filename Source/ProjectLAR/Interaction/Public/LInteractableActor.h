#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LInteractableActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UUserWidget;
class ALPlayerController;

UCLASS()
class PROJECTLAR_API ALInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	ALInteractableActor();

	void Interact(APlayerController* InteractingController);

protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnInteractionSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void SetOutlineEnabled(bool bEnabled);
	void OpenInteractionWidget(APlayerController* InteractingController);
	void CloseInteractionWidget();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TSubclassOf<UUserWidget> InteractionWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Outline")
	int32 OutlineStencilValue = 1;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> InteractionWidget;

	UPROPERTY()
	TObjectPtr<APlayerController> CachedInteractingController;

	bool bPlayerInRange = false;
};