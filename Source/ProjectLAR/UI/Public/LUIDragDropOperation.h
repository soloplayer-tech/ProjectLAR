#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "LPlayerSkillID.h"
#include "ProjectLAR/UI/Public/LUIDragDropTypes.h"

#include "LUIDragDropOperation.generated.h"

class UTexture2D;

UCLASS()
class PROJECTLAR_API ULUIDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	ELDragPayloadType PayloadType = ELDragPayloadType::None;

	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	ELPlayerSkillID SkillID = ELPlayerSkillID::None;

	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	FText DisplayName;

	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	int32 ItemID = 0;

	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	int32 ItemCount = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	bool bFromActionSlot = false;

	UPROPERTY(BlueprintReadWrite, Category = "UI Drag")
	ELActionSlotKey SourceSlotKey = ELActionSlotKey::None;
};