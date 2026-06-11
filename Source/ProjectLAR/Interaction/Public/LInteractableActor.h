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

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact(APlayerController* InteractingController);

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

	virtual void SetOutlineEnabled(bool bEnabled);
	virtual void OpenInteractionWidget(APlayerController* InteractingController);
	virtual void CloseInteractionWidget();
	virtual void OnInteractionWidgetOpened(
		UUserWidget* OpenedWidget,
		APlayerController* InteractingController
	);
	virtual void SetInteractionInputMode(
		APlayerController* InteractingController,
		UUserWidget* WidgetToFocus
	) const;
	virtual void RestoreInteractionInputMode(APlayerController* InteractingController) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
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
