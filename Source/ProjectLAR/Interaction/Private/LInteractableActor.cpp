#include "ProjectLAR/Interaction/Public/LInteractableActor.h"

#include "LPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "ProjectLAR/Player/Public/LPlayerCharacterBase.h"

ALInteractableActor::ALInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootScene);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootScene);

	InteractionSphere->SetSphereRadius(InteractionRadius);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MeshComp->SetRenderCustomDepth(false);
	MeshComp->SetCustomDepthStencilValue(OutlineStencilValue);
}

void ALInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionSphere)
	{
		InteractionSphere->SetSphereRadius(InteractionRadius);

		InteractionSphere->OnComponentBeginOverlap.AddDynamic(
			this,
			&ALInteractableActor::OnInteractionSphereBeginOverlap
		);

		InteractionSphere->OnComponentEndOverlap.AddDynamic(
			this,
			&ALInteractableActor::OnInteractionSphereEndOverlap
		);
	}

	SetOutlineEnabled(false);
}

void ALInteractableActor::OnInteractionSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	ALPlayerCharacterBase* PlayerCharacter =
		Cast<ALPlayerCharacterBase>(OtherActor);

	if (!PlayerCharacter)
	{
		return;
	}

	ALPlayerController* PlayerController =
		Cast<ALPlayerController>(PlayerCharacter->GetController());

	if (!PlayerController)
	{
		return;
	}

	bPlayerInRange = true;
	CachedInteractingController = PlayerController;

	SetOutlineEnabled(true);

	PlayerController->SetCurrentInteractable(this);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("InteractableActor Begin Overlap: %s"),
		*GetName()
	);
}

void ALInteractableActor::OnInteractionSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	ALPlayerCharacterBase* PlayerCharacter =
		Cast<ALPlayerCharacterBase>(OtherActor);

	if (!PlayerCharacter)
	{
		return;
	}

	ALPlayerController* PlayerController =
		Cast<ALPlayerController>(PlayerCharacter->GetController());

	if (PlayerController)
	{
		PlayerController->ClearCurrentInteractable(this);
	}

	bPlayerInRange = false;
	CachedInteractingController = nullptr;

	SetOutlineEnabled(false);
	CloseInteractionWidget();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("InteractableActor End Overlap: %s"),
		*GetName()
	);
}

void ALInteractableActor::Interact(APlayerController* InteractingController)
{
	if (!bPlayerInRange)
	{
		return;
	}

	if (!InteractingController)
	{
		return;
	}

	if (InteractionWidget)
	{
		CloseInteractionWidget();
	}
	else
	{
		OpenInteractionWidget(InteractingController);
	}
}

void ALInteractableActor::OpenInteractionWidget(
	APlayerController* InteractingController
)
{
	if (!InteractionWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("OpenInteractionWidget Failed: InteractionWidgetClass is null")
		);

		return;
	}

	InteractionWidget = CreateWidget<UUserWidget>(
		InteractingController,
		InteractionWidgetClass
	);

	if (!InteractionWidget)
	{
		return;
	}

	InteractionWidget->AddToViewport(80);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Interaction Widget Opened: %s"),
		*GetName()
	);
}

void ALInteractableActor::CloseInteractionWidget()
{
	if (!InteractionWidget)
	{
		return;
	}

	InteractionWidget->RemoveFromParent();
	InteractionWidget = nullptr;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Interaction Widget Closed: %s"),
		*GetName()
	);
}

void ALInteractableActor::SetOutlineEnabled(bool bEnabled)
{
	if (!MeshComp)
	{
		return;
	}

	MeshComp->SetRenderCustomDepth(bEnabled);
	MeshComp->SetCustomDepthStencilValue(OutlineStencilValue);
}