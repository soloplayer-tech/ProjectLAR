#include "ProjectLAR/Interaction/Public/LWeaponEnhanceNpcActor.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/UI/Public/LWeaponEnhanceWindowWidget.h"
#include "UObject/ConstructorHelpers.h"

ALWeaponEnhanceNpcActor::ALWeaponEnhanceNpcActor()
{
	InteractionRadius = 320.0f;

	AnvilBaseComp =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnvilBaseComp"));
	AnvilBaseComp->SetupAttachment(RootScene);

	AnvilTopComp =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnvilTopComp"));
	AnvilTopComp->SetupAttachment(RootScene);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube")
	);

	if (CubeMeshFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMeshFinder.Object);
		AnvilBaseComp->SetStaticMesh(CubeMeshFinder.Object);
		AnvilTopComp->SetStaticMesh(CubeMeshFinder.Object);
	}

	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	MeshComp->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.8f));

	AnvilBaseComp->SetRelativeLocation(FVector(90.0f, 0.0f, 35.0f));
	AnvilBaseComp->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.7f));

	AnvilTopComp->SetRelativeLocation(FVector(90.0f, 0.0f, 95.0f));
	AnvilTopComp->SetRelativeScale3D(FVector(1.5f, 0.75f, 0.25f));

	static ConstructorHelpers::FClassFinder<UUserWidget> EnhanceWindowFinder(
		TEXT("/Game/PJH/UI/WBP_WeaponEnhanceWindow")
	);

	if (EnhanceWindowFinder.Succeeded())
	{
		WeaponEnhanceWindowWidgetClass = EnhanceWindowFinder.Class;
		InteractionWidgetClass = WeaponEnhanceWindowWidgetClass;
	}
	else
	{
		WeaponEnhanceWindowWidgetClass =
			ULWeaponEnhanceWindowWidget::StaticClass();
		InteractionWidgetClass = WeaponEnhanceWindowWidgetClass;
	}
}

void ALWeaponEnhanceNpcActor::BeginPlay()
{
	if (!WeaponEnhanceWindowWidgetClass)
	{
		WeaponEnhanceWindowWidgetClass =
			ULWeaponEnhanceWindowWidget::StaticClass();
	}

	InteractionWidgetClass = WeaponEnhanceWindowWidgetClass;

	Super::BeginPlay();
}

void ALWeaponEnhanceNpcActor::Interact(
	APlayerController* InteractingController
)
{
	if (!WeaponEnhanceWindowWidgetClass)
	{
		WeaponEnhanceWindowWidgetClass =
			ULWeaponEnhanceWindowWidget::StaticClass();
	}

	InteractionWidgetClass = WeaponEnhanceWindowWidgetClass;

	Super::Interact(InteractingController);
}

void ALWeaponEnhanceNpcActor::OnInteractionWidgetOpened(
	UUserWidget* OpenedWidget,
	APlayerController* InteractingController
)
{
	ULWeaponEnhanceWindowWidget* EnhanceWindow =
		Cast<ULWeaponEnhanceWindowWidget>(OpenedWidget);

	if (!EnhanceWindow || !InteractingController)
	{
		return;
	}

	EnhanceWindow->SetObservedPlayer(
		Cast<ALPlayerCharacter>(InteractingController->GetPawn())
	);
	EnhanceWindow->BringToFront();
}

void ALWeaponEnhanceNpcActor::SetInteractionInputMode(
	APlayerController* InteractingController,
	UUserWidget* WidgetToFocus
) const
{
	if (!InteractingController || !WidgetToFocus)
	{
		return;
	}

	InteractingController->StopMovement();
	InteractingController->bShowMouseCursor = true;

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(
		InteractingController,
		WidgetToFocus,
		EMouseLockMode::DoNotLock,
		false,
		true
	);

	WidgetToFocus->SetKeyboardFocus();
}

void ALWeaponEnhanceNpcActor::SetOutlineEnabled(bool bEnabled)
{
	Super::SetOutlineEnabled(bEnabled);

	if (AnvilBaseComp)
	{
		AnvilBaseComp->SetRenderCustomDepth(bEnabled);
		AnvilBaseComp->SetCustomDepthStencilValue(OutlineStencilValue);
	}

	if (AnvilTopComp)
	{
		AnvilTopComp->SetRenderCustomDepth(bEnabled);
		AnvilTopComp->SetCustomDepthStencilValue(OutlineStencilValue);
	}
}
