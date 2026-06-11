#pragma once

#include "CoreMinimal.h"
#include "ProjectLAR/Interaction/Public/LInteractableActor.h"
#include "LBossEntranceActor.generated.h"

class UStaticMeshComponent;
class UUserWidget;

UCLASS()
class PROJECTLAR_API ALBossEntranceActor : public ALInteractableActor
{
	GENERATED_BODY()

public:
	ALBossEntranceActor();

	virtual void Interact(APlayerController* InteractingController) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnInteractionWidgetOpened(
		UUserWidget* OpenedWidget,
		APlayerController* InteractingController
	) override;
	virtual void SetOutlineEnabled(bool bEnabled) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LeftPostComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RightPostComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SignBaseComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Entrance")
	TSubclassOf<UUserWidget> BossTableWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Entrance")
	FName TargetLevelName = TEXT("OB_TestMap");
};
