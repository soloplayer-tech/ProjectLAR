#pragma once

#include "CoreMinimal.h"
#include "ProjectLAR/Interaction/Public/LInteractableActor.h"
#include "LWeaponEnhanceNpcActor.generated.h"

class UStaticMeshComponent;
class UUserWidget;

UCLASS()
class PROJECTLAR_API ALWeaponEnhanceNpcActor : public ALInteractableActor
{
	GENERATED_BODY()

public:
	ALWeaponEnhanceNpcActor();

	virtual void Interact(APlayerController* InteractingController) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnInteractionWidgetOpened(
		UUserWidget* OpenedWidget,
		APlayerController* InteractingController
	) override;
	virtual void SetOutlineEnabled(bool bEnabled) override;
	virtual void SetInteractionInputMode(
		APlayerController* InteractingController,
		UUserWidget* WidgetToFocus
	) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> AnvilBaseComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> AnvilTopComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Enhance")
	TSubclassOf<UUserWidget> WeaponEnhanceWindowWidgetClass;
};
