#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OB_BossAnimInstance.generated.h"

// 몽타주 재생이 끝났음을 다른 클래스(FSM 등)에 알리기 위한 델리게이트 선언
DECLARE_MULTICAST_DELEGATE(FOnMontageEndedDelegate);

UCLASS()
class PROJECTLAR_API UOb_BossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UOb_BossAnimInstance();

	// 블루프린트의 '이벤트 블루프린트 업데이트 애니메이션' 노드와 완벽히 같은 역할
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 공격 몽타주를 재생하는 함수
	void PlayAttackMontage(UAnimMontage* MontageToPlay);

	// 외부에서 구독할 수 있는 종료 알림 델리게이트
	FOnMontageEndedDelegate OnAttackMontageEnded;

protected:
	// 엔진 내부에서 몽타주가 끝날 때 자동으로 호출되는 델리게이트에 바인딩할 함수
	UFUNCTION()
	void AnimNotify_MontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 블루프린트(AnimGraph)에서 마음껏 꺼내 쓸 수 있도록 매크로 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossAnim")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossAnim")
	float Direction;
};