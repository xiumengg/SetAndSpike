// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionQuality.h"

#include "Components/ProgressBar.h"

void UActionQuality::NativeConstruct()
{
	Super::NativeConstruct();
	

	if(ActionQualityBar)
	{
		ActionQualityBar->SetPercent(0.1);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("%d"), bHasScriptImplementedTick);
}

void UActionQuality::SetActionQualityBarPercent(float Percent)
{
	ActionQualityBar->SetPercent(Percent);
}

