// Fill out your copyright notice in the Description page of Project Settings.


#include "./../Public/LPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/LocalPlayer.h"
#include "ProjectLAR/Player/Public/LPlayerCharacterBase.h"

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
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
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
	}
}

void ALPlayerController::MoveToMouseCursor()
{
	FVector TargetLocation;
	
	if (!GetMouseWorldLocation(TargetLocation))
	{
		return;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetLocation);
}

void ALPlayerController::DashInput()
{
	ALPlayerCharacterBase* PlayerCharacter = Cast<ALPlayerCharacterBase>(GetPawn());
	
	if (!PlayerCharacter)
	{
		return;
	}
	
	FVector MouseWorldLocation;
	
	if (!GetMouseWorldLocation(MouseWorldLocation))
	{
		return;
	}
	
	FVector DashDirection = MouseWorldLocation - PlayerCharacter->GetActorLocation();
	
	DashDirection.Z = 0.0f;
	
	if (DashDirection.IsNearlyZero())
	{
		return;
	}
	
	DashDirection.Normalize();
	
	StopMovement();
	
	PlayerCharacter->Dash(DashDirection);
}

bool ALPlayerController::GetMouseWorldLocation(FVector& OutWorldLocation) const
{
	FHitResult HitResult;
	
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	
	if (!bHit)
	{
		return false;
	}
	
	OutWorldLocation = HitResult.Location;
	return true;
}


/*FHitResult HitResult;
	
	// 마우스 커서 아래의 월드 위치를 찾는다
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	
	if (bHit)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitResult.Location);
	}
}*/

