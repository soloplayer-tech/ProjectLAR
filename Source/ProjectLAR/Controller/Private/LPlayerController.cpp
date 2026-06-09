// Fill out your copyright notice in the Description page of Project Settings.


#include "LPlayerController.h"
#include "ProjectLAR/Interaction/Public/LInteractableActor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LPlayerUIWidget.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/LocalPlayer.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/UI/Public/LSkillWindowWidget.h"
#include "ProjectLAR/Player/Public/LPlayerCharacterBase.h"

#define ECC_SkillTarget ECC_GameTraceChannel1

ALPlayerController::ALPlayerController()
{
	// 마우스 커서 보이게 만들기
	bShowMouseCursor = true;
	
	// 마우스 클릭 관련 기능 쓸 수 있도록 만들기 위해서
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ALPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	CreatePlayerUIWidget();
	CreateSkillWindowWidget();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Player)
			{
				Subsystem->AddMappingContext(IMC_Player, 0);
			}
		}
	}
}

void ALPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ClickMoveAction)
		{
			EnhancedInput->BindAction(
				ClickMoveAction,
				ETriggerEvent::Triggered,
				this,
				&ALPlayerController::MoveToMouseCursor
			);
		}
		
		if (PlayerDashAction)
		{
			EnhancedInput->BindAction(
				PlayerDashAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::DashInput
			);
		}
		
		if (BasicAttackAction)
		{
			EnhancedInput->BindAction(
				BasicAttackAction,
				ETriggerEvent::Triggered,
				this,
				&ALPlayerController::BasicAttackInput
			);
		}
		
		if (SkillQAction)
		{
			EnhancedInput->BindAction(
				SkillQAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillQInput
				);
		}
		
		if (SkillWAction)
		{
			EnhancedInput->BindAction(
				SkillWAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillWInput
				);
		}
		
		if (SkillEAction)
		{
			EnhancedInput->BindAction(
				SkillEAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillEInput
				);
		}
		
		if (SkillRAction)
		{
			EnhancedInput->BindAction(
				SkillRAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillRInput
				);
		}
		
		if (SkillAAction)
		{
			EnhancedInput->BindAction(
				SkillAAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillAInput
				);
		}
		
		if (SkillSAction)
		{
			EnhancedInput->BindAction(
				SkillSAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillSInput
				);
		}
		
		if (SkillDAction)
		{
			EnhancedInput->BindAction(
				SkillDAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillDInput
				);
		}
		
		if (SkillFAction)
		{
			EnhancedInput->BindAction(
				SkillFAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillFInput
				);
		}
		
		if (SkillVAction)
		{
			EnhancedInput->BindAction(
				SkillVAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::SkillVInput
				);
		}
		
		if (IdentityAction)
		{
			EnhancedInput->BindAction(
				IdentityAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::IdentityInput
			);
		}
		
		if (CameraZoomInAction)
		{
			EnhancedInput->BindAction(
				CameraZoomInAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::CameraZoomInInput
			);
		}

		if (CameraZoomOutAction)
		{
			EnhancedInput->BindAction(
				CameraZoomOutAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::CameraZoomOutInput
			);
		}
		
		if (ToggleSkillWindowAction)
		{
			EnhancedInput->BindAction(
				ToggleSkillWindowAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::ToggleSkillWindowInput
			);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(
				InteractAction,
				ETriggerEvent::Started,
				this,
				&ALPlayerController::InteractInput
			);
		}
	}
}

void ALPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	BindPlayerUIToPawn(InPawn);
	BindSkillWindowToPawn(InPawn);
}

void ALPlayerController::IdentityInput()
{
	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(GetPawn());

	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->ActivateIdentity();
}

void ALPlayerController::CameraZoomInInput()
{
	ALPlayerCharacterBase* PlayerCharacter =
		Cast<ALPlayerCharacterBase>(GetPawn());

	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->CameraZoomIn();
}

void ALPlayerController::CameraZoomOutInput()
{
	ALPlayerCharacterBase* PlayerCharacter =
		Cast<ALPlayerCharacterBase>(GetPawn());

	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->CameraZoomOut();
}

void ALPlayerController::CreatePlayerUIWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("CreatePlayerUIWidget Called"));

	if (PlayerUIWidget)
	{
		BindPlayerUIToPawn(GetPawn());
		return;
	}
	
	if (!IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("CreatePlayerUIWidget Failed: Not Local Controller"));
		return;
	}

	if (!PlayerUIWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreatePlayerUIWidget Failed: PlayerUIWidgetClass is null"));
		return;
	}

	PlayerUIWidget = CreateWidget<ULPlayerUIWidget>(
		this,
		PlayerUIWidgetClass
	);

	if (!PlayerUIWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreatePlayerUIWidget Failed: CreateWidget returned null"));
		return;
	}

	PlayerUIWidget->AddToViewport(100);

	UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget AddToViewport Success"));

	BindPlayerUIToPawn(GetPawn());
}

void ALPlayerController::BindPlayerUIToPawn(APawn* InPawn)
{
	if (!PlayerUIWidget)
	{
		return;
	}

	PlayerUIWidget->SetObservedCharacter(
		Cast<ALPlayerCharacterBase>(InPawn)
	);
}

void ALPlayerController::MoveToMouseCursor()
{
	ALPlayerCharacterBase* PlayerCharacter =
		Cast<ALPlayerCharacterBase>(GetPawn());

	if (!PlayerCharacter)
	{
		return;
	}

	if (!PlayerCharacter->CanMove())
	{
		return;
	}

	PlayerCharacter->CancelCurrentActionFor(ELPlayerActionCommand::Move);

	FVector TargetLocation;
	
	if (!GetMouseWorldLocation(TargetLocation))
	{
		return;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetLocation);
}

void ALPlayerController::DashInput()
{
	ALPlayerCharacterBase* PlayerCharacter =
		Cast<ALPlayerCharacterBase>(GetPawn());
	
	if (!PlayerCharacter)
	{
		return;
	}

	// 현재 상태에서 대쉬가 가능한지 확인
	if (!PlayerCharacter->CanDash())
	{
		return;
	}
	
	FVector MouseWorldLocation;
	
	if (!GetMouseWorldLocation(MouseWorldLocation))
	{
		return;
	}
	
	FVector DashDirection =
		MouseWorldLocation - PlayerCharacter->GetActorLocation();
	
	DashDirection.Z = 0.0f;
	
	if (DashDirection.IsNearlyZero())
	{
		return;
	}
	
	DashDirection.Normalize();
	
	StopMovement();

	PlayerCharacter->CancelCurrentActionFor(ELPlayerActionCommand::Dash);
	
	PlayerCharacter->Dash(DashDirection);
}

void ALPlayerController::BasicAttackInput()
{
	ALPlayerCharacter* PlayerCharacter = Cast<ALPlayerCharacter>(GetPawn());
	
	if (!PlayerCharacter)
	{
		return;
	}
	
	if (!PlayerCharacter->CanStartAction(ELPlayerActionCommand::BasicAttack))
	{
		return;
	}
	
	FVector MouseWorldLocation;
	
	if (!GetMouseWorldLocation(MouseWorldLocation))
	{
		return;
	}
	
	StopMovement();
	
	PlayerCharacter->BasicAttack(MouseWorldLocation);
}

bool ALPlayerController::GetMouseWorldLocation(FVector& OutWorldLocation) const
{
	FHitResult HitResult;
	
	const bool bHit = GetHitResultUnderCursor(ECC_SkillTarget, false, HitResult);
	
	if (!bHit)
	{
		return false;
	}
	
	OutWorldLocation = HitResult.Location;
	return true;
}

void ALPlayerController::SkillQInput()
{
	HandleSkillInput(ELPlayerSkillSlot::Q);
}

void ALPlayerController::SkillWInput()
{
	HandleSkillInput(ELPlayerSkillSlot::W);
}

void ALPlayerController::SkillEInput()
{
	HandleSkillInput(ELPlayerSkillSlot::E);
}

void ALPlayerController::SkillRInput()
{
	HandleSkillInput(ELPlayerSkillSlot::R);
}

void ALPlayerController::SkillAInput()
{
	HandleSkillInput(ELPlayerSkillSlot::A);
}

void ALPlayerController::SkillSInput()
{
	HandleSkillInput(ELPlayerSkillSlot::S);
}

void ALPlayerController::SkillDInput()
{
	HandleSkillInput(ELPlayerSkillSlot::D);
}

void ALPlayerController::SkillFInput()
{
	HandleSkillInput(ELPlayerSkillSlot::F);
}

void ALPlayerController::SkillVInput()
{
	HandleSkillInput(ELPlayerSkillSlot::V);

}

void ALPlayerController::HandleSkillInput(ELPlayerSkillSlot SkillSlot)
{
	ALPlayerCharacter* PlayerCharacter = Cast<ALPlayerCharacter>(GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}
	
	// 현재 상태에서 스킬 사용이 가능한지 확인
	if (!PlayerCharacter->CanUseSkillSlot(SkillSlot))
	{
		return;
	}
	
	FVector MouseWorldLocation;
	
	if (!GetMouseWorldLocation(MouseWorldLocation))
	{
		return;
	}
	
	StopMovement();
	
	PlayerCharacter->UseSkill(SkillSlot, MouseWorldLocation);
}


void ALPlayerController::CreateSkillWindowWidget()
{
	if (SkillWindowWidget)
	{
		BindSkillWindowToPawn(GetPawn());
		return;
	}

	if (!IsLocalController())
	{
		return;
	}

	if (!SkillWindowWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateSkillWindowWidget Failed: SkillWindowWidgetClass is null"));
		return;
	}

	SkillWindowWidget = CreateWidget<ULSkillWindowWidget>(
		this,
		SkillWindowWidgetClass
	);

	if (!SkillWindowWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateSkillWindowWidget Failed: CreateWidget returned null"));
		return;
	}

	SkillWindowWidget->AddToViewport(50);

	// 처음에는 숨김
	SkillWindowWidget->SetVisibility(ESlateVisibility::Collapsed);

	BindSkillWindowToPawn(GetPawn());

	UE_LOG(LogTemp, Warning, TEXT("SkillWindowWidget Created"));
}

void ALPlayerController::BindSkillWindowToPawn(APawn* InPawn)
{
	if (!SkillWindowWidget)
	{
		return;
	}

	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(InPawn);

	SkillWindowWidget->SetOwningPlayerCharacter(PlayerCharacter);
}

void ALPlayerController::ToggleSkillWindowInput()
{
	if (!SkillWindowWidget)
	{
		CreateSkillWindowWidget();
	}

	if (!SkillWindowWidget)
	{
		return;
	}

	const bool bCurrentlyVisible =
		SkillWindowWidget->GetVisibility() != ESlateVisibility::Collapsed;

	SkillWindowWidget->SetVisibility(
		bCurrentlyVisible
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible
	);
}

/*FHitResult HitResult;
	
	// 마우스 커서 아래의 월드 위치를 찾는다
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	
	if (bHit)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitResult.Location);
	}
}*/

void ALPlayerController::SetCurrentInteractable(
	ALInteractableActor* InInteractable
)
{
	CurrentInteractable = InInteractable;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("SetCurrentInteractable: %s"),
		CurrentInteractable ? *CurrentInteractable->GetName() : TEXT("None")
	);
}

void ALPlayerController::ClearCurrentInteractable(
	ALInteractableActor* InInteractable
)
{
	if (CurrentInteractable != InInteractable)
	{
		return;
	}

	CurrentInteractable = nullptr;

	UE_LOG(LogTemp, Warning, TEXT("ClearCurrentInteractable"));
}

void ALPlayerController::InteractInput()
{
	if (!CurrentInteractable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact Failed: No CurrentInteractable"));
		return;
	}

	CurrentInteractable->Interact(this);
}
