// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionQuality.h"

#include "Components/ProgressBar.h"

void UActionQuality::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UActionQuality::SetActionQualityBarPercent(float Percent)
{
	ActionQualityBar->SetPercent(Percent);
}

