// ScreenMessage.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScreenMessage.generated.h"

class UTextBlock;

/**
 * HUD message widget.
 */
UCLASS()
class BATTLEBLASTER_API UScreenMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageTextBlock;

	void SetMessageText(const FString& Text);
};