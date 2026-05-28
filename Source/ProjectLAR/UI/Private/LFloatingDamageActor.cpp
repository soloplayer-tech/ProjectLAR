#include "ProjectLAR/UI/Public/LFloatingDamageActor.h"

#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "ProjectLAR/UI/Public/LFloatingDamageWidget.h"

ALFloatingDamageActor::ALFloatingDamageActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidgetComponent"));
	DamageWidgetComponent->SetupAttachment(RootScene);

	// 화면을 향하는 UI 방식
	DamageWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidgetComponent->SetDrawSize(FVector2D(120.0f, 60.0f));
	DamageWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALFloatingDamageActor::BeginPlay()
{
	Super::BeginPlay();

	const FVector RandomOffset(
		FMath::RandRange(-RandomHorizontalOffset, RandomHorizontalOffset),
		FMath::RandRange(-RandomHorizontalOffset, RandomHorizontalOffset),
		0.0f
	);

	AddActorWorldOffset(RandomOffset);
}

void ALFloatingDamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	AddActorWorldOffset(
		FVector(0.0f, 0.0f, FloatSpeed * DeltaTime)
	);

	if (ElapsedTime >= LifeTime)
	{
		Destroy();
	}
}

void ALFloatingDamageActor::InitializeFloatingDamage(float Damage)
{
	if (!DamageWidgetComponent)
	{
		return;
	}

	ULFloatingDamageWidget* DamageWidget =
		Cast<ULFloatingDamageWidget>(DamageWidgetComponent->GetUserWidgetObject());

	if (!DamageWidget)
	{
		return;
	}

	DamageWidget->SetDamageValue(Damage);
}