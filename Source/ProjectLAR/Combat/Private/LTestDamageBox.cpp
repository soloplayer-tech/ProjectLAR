#include "ProjectLAR/Combat/Public/LTestDamageBox.h"

#include "LPlayerSkillID.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ALTestDamageBox::ALTestDamageBox()
{
	PrimaryActorTick.bCanEverTick = false;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	SetRootComponent(HitBox);

	HitBox->SetBoxExtent(FVector(50.0f, 50.0f, 100.0f));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HitBox->SetCollisionObjectType(ECC_WorldDynamic);
	HitBox->SetCollisionResponseToAllChannels(ECR_Block);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(HitBox);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALTestDamageBox::BeginPlay()
{
	Super::BeginPlay();

	CurHP = MaxHP;
}

void ALTestDamageBox::ReceiveSkillDamage_Implementation(float Damage, AActor* DamageCauser, ELPlayerSkillSlot SkillID)
{
	if (CurHP <= 0.0f)
	{
		return;
	}

	CurHP -= Damage;
	CurHP = FMath::Max(0.0f, CurHP);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("TestDamageBox Hit! Skill: %s / Damage: %.1f / HP: %.1f / %.1f / Causer: %s"),
		*UEnum::GetValueAsString(SkillID),
		Damage,
		CurHP,
		MaxHP,
		DamageCauser ? *DamageCauser->GetName() : TEXT("None")
	);

	if (CurHP <= 0.0f)
	{
		Die();
	}
}

void ALTestDamageBox::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("TestDamageBox Dead"));

	if (bDestroyOnDeath)
	{
		Destroy();
	}
}