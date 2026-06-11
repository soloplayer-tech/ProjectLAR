#include "OB_BossAnimInstance.h"

UOb_BossAnimInstance::UOb_BossAnimInstance()
{
	Speed = 0.0f;
	Direction = 0.0f;
}

void UOb_BossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 보스 캐릭터 정보 가져오기 (블루프린트의 Try Get Pawn Owner 역할)
	APawn* Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		FVector Velocity = Pawn->GetVelocity();
		Speed = Velocity.Length();

		FRotator Rotation = Pawn->GetActorRotation();
		Direction = CalculateDirection(Velocity, Rotation);
	}
}

void UOb_BossAnimInstance::PlayAttackMontage(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay)
	{
		Montage_Play(MontageToPlay);

		if (!OnMontageEnded.IsAlreadyBound(this, &UOb_BossAnimInstance::AnimNotify_MontageEnded))
		{
			OnMontageEnded.AddDynamic(this, &UOb_BossAnimInstance::AnimNotify_MontageEnded);
		}
	}
}

void UOb_BossAnimInstance::AnimNotify_MontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackMontageEnded.Broadcast();
}