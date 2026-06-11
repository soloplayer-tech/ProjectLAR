#include "ProjectLAR/Interaction/Public/LBossEntranceActor.h"

#include "Components/StaticMeshComponent.h"
#include "ProjectLAR/UI/Public/LBossTableWidget.h"
#include "UObject/ConstructorHelpers.h"

ALBossEntranceActor::ALBossEntranceActor()
{
	InteractionRadius = 350.0f;

	LeftPostComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftPostComp"));
	LeftPostComp->SetupAttachment(RootScene);

	RightPostComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPostComp"));
	RightPostComp->SetupAttachment(RootScene);

	SignBaseComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SignBaseComp"));
	SignBaseComp->SetupAttachment(RootScene);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube")
	);

	if (CubeMeshFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMeshFinder.Object);
		LeftPostComp->SetStaticMesh(CubeMeshFinder.Object);
		RightPostComp->SetStaticMesh(CubeMeshFinder.Object);
		SignBaseComp->SetStaticMesh(CubeMeshFinder.Object);
	}

	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	MeshComp->SetRelativeScale3D(FVector(2.8f, 0.12f, 0.85f));

	LeftPostComp->SetRelativeLocation(FVector(115.0f, 0.0f, 80.0f));
	LeftPostComp->SetRelativeScale3D(FVector(0.12f, 0.12f, 1.6f));

	RightPostComp->SetRelativeLocation(FVector(-115.0f, 0.0f, 80.0f));
	RightPostComp->SetRelativeScale3D(FVector(0.12f, 0.12f, 1.6f));

	SignBaseComp->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
	SignBaseComp->SetRelativeScale3D(FVector(3.2f, 0.18f, 0.1f));

	static ConstructorHelpers::FClassFinder<UUserWidget> BossTableWidgetFinder(
		TEXT("/Game/PJH/UI/WBP_BossTable")
	);

	if (BossTableWidgetFinder.Succeeded())
	{
		BossTableWidgetClass = BossTableWidgetFinder.Class;
		InteractionWidgetClass = BossTableWidgetClass;
	}
}

void ALBossEntranceActor::BeginPlay()
{
	if (BossTableWidgetClass)
	{
		InteractionWidgetClass = BossTableWidgetClass;
	}

	Super::BeginPlay();
}

void ALBossEntranceActor::Interact(APlayerController* InteractingController)
{
	if (BossTableWidgetClass)
	{
		InteractionWidgetClass = BossTableWidgetClass;
	}

	Super::Interact(InteractingController);
}

void ALBossEntranceActor::OnInteractionWidgetOpened(
	UUserWidget* OpenedWidget,
	APlayerController* InteractingController
)
{
	if (ULBossTableWidget* BossTableWidget = Cast<ULBossTableWidget>(OpenedWidget))
	{
		BossTableWidget->SetTargetLevelName(TargetLevelName);
	}
}

void ALBossEntranceActor::SetOutlineEnabled(bool bEnabled)
{
	Super::SetOutlineEnabled(bEnabled);

	if (LeftPostComp)
	{
		LeftPostComp->SetRenderCustomDepth(bEnabled);
		LeftPostComp->SetCustomDepthStencilValue(OutlineStencilValue);
	}

	if (RightPostComp)
	{
		RightPostComp->SetRenderCustomDepth(bEnabled);
		RightPostComp->SetCustomDepthStencilValue(OutlineStencilValue);
	}

	if (SignBaseComp)
	{
		SignBaseComp->SetRenderCustomDepth(bEnabled);
		SignBaseComp->SetCustomDepthStencilValue(OutlineStencilValue);
	}
}
