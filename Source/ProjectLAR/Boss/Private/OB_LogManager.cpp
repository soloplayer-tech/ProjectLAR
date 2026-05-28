#include "OB_LogManager.h"

#include "CoreMinimal.h"

DEFINE_LOG_CATEGORY(LogIntegrated);
DEFINE_LOG_CATEGORY(LogGameplay);

void OB_LogManager::LogAndScreen(const FString& Message, float DisplayTime, FColor Color)
{
	// 1. 출력 창(Output Log)에 기록
	LOG_TRACE_INFO(TEXT("%s"), *Message);

	// 2. 게임 화면(Viewport)에 즉시 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, DisplayTime, Color, Message);
	}
}