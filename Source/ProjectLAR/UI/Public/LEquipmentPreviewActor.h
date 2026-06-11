#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LEquipmentPreviewActor.generated.h"

class ALPlayerCharacter;
class USceneCaptureComponent2D;
class USceneComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UTextureRenderTarget2D;
class UNiagaraComponent;
class UPointLightComponent;

UCLASS()
class PROJECTLAR_API ALEquipmentPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ALEquipmentPreviewActor();

	void SetupFromPlayer(ALPlayerCharacter* PlayerCharacter);
	void SetRenderTarget(UTextureRenderTarget2D* InRenderTarget);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> PreviewMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PreviewWeaponMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> PreviewWeaponNiagaraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> KeyLightComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> FillLightComp;

	// 캐릭터 뒤쪽에 깔 어두운 배경판
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BackgroundPlaneComp;

private:
	void ApplyWeaponVisual(ALPlayerCharacter* PlayerCharacter);
};