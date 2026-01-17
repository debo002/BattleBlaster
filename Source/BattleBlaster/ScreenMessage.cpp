// ScreenMessage.cpp
#include "ScreenMessage.h"
#include "Components/TextBlock.h"

void UScreenMessage::SetMessageText(const FString& Text)
{
	if (MessageTextBlock)
	{
		MessageTextBlock->SetText(FText::FromString(Text));
	}
}
