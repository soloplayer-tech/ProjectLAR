#pragma once

#include "CoreMinimal.h"
#include "LUIDragDropTypes.generated.h"

// 드래그 중인 데이터가 무엇인지 구분
UENUM(BlueprintType)
enum class ELDragPayloadType : uint8
{
	None UMETA(DisplayName = "None"),

	Skill UMETA(DisplayName = "Skill"),
	Ultimate UMETA(DisplayName = "Ultimate"),
	Item UMETA(DisplayName = "Item")
};

// 슬롯이 어떤 종류의 데이터를 받을 수 있는지 구분
UENUM(BlueprintType)
enum class ELActionSlotType : uint8
{
	None UMETA(DisplayName = "None"),

	Skill UMETA(DisplayName = "Skill"),
	Ultimate UMETA(DisplayName = "Ultimate"),
	Item UMETA(DisplayName = "Item")
};

// 실제 슬롯 위치 구분
UENUM(BlueprintType)
enum class ELActionSlotKey : uint8
{
	None UMETA(DisplayName = "None"),

	// 일반 스킬 슬롯
	Skill_Q UMETA(DisplayName = "Skill Q"),
	Skill_W UMETA(DisplayName = "Skill W"),
	Skill_E UMETA(DisplayName = "Skill E"),
	Skill_R UMETA(DisplayName = "Skill R"),
	Skill_A UMETA(DisplayName = "Skill A"),
	Skill_S UMETA(DisplayName = "Skill S"),
	Skill_D UMETA(DisplayName = "Skill D"),
	Skill_F UMETA(DisplayName = "Skill F"),

	// 궁극기 슬롯
	Ultimate_V UMETA(DisplayName = "Ultimate V"),

	// 아이템 / 포션 슬롯
	Item_1 UMETA(DisplayName = "Item 1"),
	Item_2 UMETA(DisplayName = "Item 2"),
	Item_3 UMETA(DisplayName = "Item 3"),
	Item_4 UMETA(DisplayName = "Item 4")
};