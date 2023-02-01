// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionQuality.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SETANDSPIKE_API UActionQuality : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetActionQualityBarPercent(float Percent);
	
protected:
	virtual void NativeConstruct() override;

	

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* ActionQualityBar;

	
};
