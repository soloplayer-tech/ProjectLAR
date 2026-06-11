#include "ProjectLAR/UI/Public/LEquipmentPreviewActor.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "ProjectLAR/Item/Public/LInventoryComponent.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALEquipmentPreviewActor::ALEquipmentPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorEnableCollision(false);

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	PreviewMeshComp =
		CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMeshComp"));
	PreviewMeshComp->SetupAttachment(RootScene);
	PreviewMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMeshComp->SetGenerateOverlapEvents(false);
	PreviewMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	PreviewMeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	PreviewMeshComp->SetCastShadow(false);
	PreviewMeshComp->SetHiddenInGame(false);
	PreviewMeshComp->SetVisibility(true, true);

	PreviewWeaponMeshComp =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewWeaponMeshComp"));
	PreviewWeaponMeshComp->SetupAttachment(PreviewMeshComp);
	PreviewWeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewWeaponMeshComp->SetGenerateOverlapEvents(false);
	PreviewWeaponMeshComp->SetCastShadow(false);
	PreviewWeaponMeshComp->SetHiddenInGame(true);
	PreviewWeaponMeshComp->SetVisibility(false, true);

	PreviewWeaponNiagaraComp =
		CreateDefaultSubobject<UNiagaraComponent>(TEXT("PreviewWeaponNiagaraComp"));
	PreviewWeaponNiagaraComp->SetupAttachment(PreviewMeshComp);
	PreviewWeaponNiagaraComp->SetAutoActivate(false);
	PreviewWeaponNiagaraComp->SetHiddenInGame(true);
	PreviewWeaponNiagaraComp->SetVisibility(false, true);

	// 캐릭터 뒤쪽 배경판
	// SceneCapture가 하늘/안개를 찍는 대신 이 판을 찍게 해서 캐릭터가 묻히지 않게 함
	BackgroundPlaneComp =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackgroundPlaneComp"));
	BackgroundPlaneComp->SetupAttachment(RootScene);
	BackgroundPlaneComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BackgroundPlaneComp->SetGenerateOverlapEvents(false);
	BackgroundPlaneComp->SetCastShadow(false);
	BackgroundPlaneComp->SetHiddenInGame(false);
	BackgroundPlaneComp->SetVisibility(true, true);

	// 카메라가 X=320 쪽에서 원점을 바라보므로,
	// 캐릭터 뒤쪽인 X 음수 방향에 배경판 배치
	BackgroundPlaneComp->SetRelativeLocation(FVector(-90.0f, 0.0f, 95.0f));
	BackgroundPlaneComp->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));
	BackgroundPlaneComp->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshFinder(
		TEXT("/Engine/BasicShapes/Plane.Plane")
	);

	if (PlaneMeshFinder.Succeeded())
	{
		BackgroundPlaneComp->SetStaticMesh(PlaneMeshFinder.Object);
	}

	SceneCaptureComp =
		CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComp"));
	SceneCaptureComp->SetupAttachment(RootScene);

	SceneCaptureComp->SetRelativeLocation(FVector(320.0f, 0.0f, 112.0f));
	SceneCaptureComp->SetRelativeRotation(
		UKismetMathLibrary::FindLookAtRotation(
			SceneCaptureComp->GetRelativeLocation(),
			FVector(0.0f, 0.0f, 95.0f)
		)
	);

	SceneCaptureComp->FOVAngle = 30.0f;
	SceneCaptureComp->CaptureSource = SCS_FinalColorLDR;
	SceneCaptureComp->bCaptureEveryFrame = true;
	SceneCaptureComp->bCaptureOnMovement = false;

	// 이 액터 안에 있는 컴포넌트만 찍도록 제한
	SceneCaptureComp->PrimitiveRenderMode =
		ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComp->ShowOnlyActors.Add(this);

	// 월드의 하늘, 안개, 대기효과가 프리뷰에 섞이지 않게 제거
	SceneCaptureComp->ShowFlags.SetAtmosphere(false);
	SceneCaptureComp->ShowFlags.SetFog(false);
	SceneCaptureComp->ShowFlags.SetSkyLighting(false);

	// 너무 밝게 날아가는 문제 방지
	SceneCaptureComp->PostProcessSettings.bOverride_AutoExposureBias = true;
	SceneCaptureComp->PostProcessSettings.AutoExposureBias = -1.5f;

	KeyLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("KeyLightComp"));
	KeyLightComp->SetupAttachment(RootScene);
	KeyLightComp->SetRelativeLocation(FVector(240.0f, -120.0f, 230.0f));

	// 기존 18000은 너무 강함
	KeyLightComp->SetIntensity(700.0f);
	KeyLightComp->SetAttenuationRadius(500.0f);
	KeyLightComp->SetCastShadows(false);

	FillLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("FillLightComp"));
	FillLightComp->SetupAttachment(RootScene);
	FillLightComp->SetRelativeLocation(FVector(140.0f, 180.0f, 160.0f));

	// 기존 7000은 너무 강함
	FillLightComp->SetIntensity(200.0f);
	FillLightComp->SetAttenuationRadius(500.0f);
	FillLightComp->SetCastShadows(false);
}

void ALEquipmentPreviewActor::SetRenderTarget(
	UTextureRenderTarget2D* InRenderTarget
)
{
	if (SceneCaptureComp)
	{
		SceneCaptureComp->TextureTarget = InRenderTarget;
		SceneCaptureComp->CaptureScene();
	}
}

void ALEquipmentPreviewActor::SetupFromPlayer(ALPlayerCharacter* PlayerCharacter)
{
	if (!PlayerCharacter || !PreviewMeshComp)
	{
		return;
	}

	USkeletalMeshComponent* SourceMeshComp = PlayerCharacter->GetMesh();

	if (!SourceMeshComp)
	{
		return;
	}

	PreviewMeshComp->SetSkeletalMesh(SourceMeshComp->GetSkeletalMeshAsset());
	PreviewMeshComp->SetAnimInstanceClass(SourceMeshComp->GetAnimClass());
	PreviewMeshComp->SetHiddenInGame(false);
	PreviewMeshComp->SetVisibility(true, true);

	for (int32 MaterialIndex = 0; MaterialIndex < SourceMeshComp->GetNumMaterials(); ++MaterialIndex)
	{
		PreviewMeshComp->SetMaterial(
			MaterialIndex,
			SourceMeshComp->GetMaterial(MaterialIndex)
		);
	}

	ApplyWeaponVisual(PlayerCharacter);

	if (SceneCaptureComp)
	{
		SceneCaptureComp->CaptureScene();
	}
}

void ALEquipmentPreviewActor::ApplyWeaponVisual(
	ALPlayerCharacter* PlayerCharacter
)
{
	if (!PlayerCharacter || !PreviewWeaponMeshComp || !PreviewWeaponNiagaraComp)
	{
		return;
	}

	ULInventoryComponent* InventoryComponent =
		PlayerCharacter->GetInventoryComponent();

	FLInventorySlot EquippedWeaponSlot;
	const bool bHasEquippedWeapon =
		InventoryComponent
		&& InventoryComponent->GetEquippedWeaponSlot(EquippedWeaponSlot)
		&& EquippedWeaponSlot.ItemData;

	if (!bHasEquippedWeapon)
	{
		PreviewWeaponMeshComp->SetStaticMesh(nullptr);
		PreviewWeaponMeshComp->SetHiddenInGame(true);
		PreviewWeaponMeshComp->SetVisibility(false, true);

		PreviewWeaponNiagaraComp->Deactivate();
		PreviewWeaponNiagaraComp->SetAsset(nullptr);
		PreviewWeaponNiagaraComp->SetHiddenInGame(true);
		PreviewWeaponNiagaraComp->SetVisibility(false, true);
		return;
	}

	UStaticMeshComponent* SourceWeaponMeshComp =
		PlayerCharacter->GetWeaponVisualStaticMeshComponent();

	UNiagaraComponent* SourceWeaponNiagaraComp =
		PlayerCharacter->GetWeaponVisualNiagaraComponent();

	if (SourceWeaponMeshComp && SourceWeaponMeshComp->GetStaticMesh())
	{
		FName SocketToUse = SourceWeaponMeshComp->GetAttachSocketName();

		if (!SocketToUse.IsNone() && !PreviewMeshComp->DoesSocketExist(SocketToUse))
		{
			SocketToUse = NAME_None;
		}

		PreviewWeaponMeshComp->AttachToComponent(
			PreviewMeshComp,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketToUse
		);

		PreviewWeaponMeshComp->SetRelativeTransform(
			SourceWeaponMeshComp->GetRelativeTransform()
		);

		PreviewWeaponMeshComp->SetStaticMesh(
			SourceWeaponMeshComp->GetStaticMesh()
		);

		for (int32 MaterialIndex = 0; MaterialIndex < SourceWeaponMeshComp->GetNumMaterials(); ++MaterialIndex)
		{
			PreviewWeaponMeshComp->SetMaterial(
				MaterialIndex,
				SourceWeaponMeshComp->GetMaterial(MaterialIndex)
			);
		}

		PreviewWeaponMeshComp->SetHiddenInGame(false);
		PreviewWeaponMeshComp->SetVisibility(true, true);
	}
	else
	{
		PreviewWeaponMeshComp->SetStaticMesh(nullptr);
		PreviewWeaponMeshComp->SetHiddenInGame(true);
		PreviewWeaponMeshComp->SetVisibility(false, true);
	}

	if (SourceWeaponNiagaraComp && SourceWeaponNiagaraComp->GetAsset())
	{
		USceneComponent* PreviewNiagaraParent = PreviewMeshComp;

		if (SourceWeaponNiagaraComp->GetAttachParent() == SourceWeaponMeshComp
			&& PreviewWeaponMeshComp->GetStaticMesh())
		{
			PreviewNiagaraParent = PreviewWeaponMeshComp;
		}

		PreviewWeaponNiagaraComp->AttachToComponent(
			PreviewNiagaraParent,
			FAttachmentTransformRules::KeepRelativeTransform
		);

		PreviewWeaponNiagaraComp->SetRelativeTransform(
			SourceWeaponNiagaraComp->GetRelativeTransform()
		);

		PreviewWeaponNiagaraComp->SetAsset(
			SourceWeaponNiagaraComp->GetAsset()
		);

		PreviewWeaponNiagaraComp->SetHiddenInGame(false);
		PreviewWeaponNiagaraComp->SetVisibility(true, true);
		PreviewWeaponNiagaraComp->Activate(true);
	}
	else
	{
		PreviewWeaponNiagaraComp->Deactivate();
		PreviewWeaponNiagaraComp->SetAsset(nullptr);
		PreviewWeaponNiagaraComp->SetHiddenInGame(true);
		PreviewWeaponNiagaraComp->SetVisibility(false, true);
	}
}