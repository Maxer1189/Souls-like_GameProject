// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/EnemyUIComponent.h"

#include "UserInterface/HUD/CharacterWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawnWidget(UCharacterWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnWidgets()
{
	if (EnemyDrawnWidgets.IsEmpty())
		return;

	for (UCharacterWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}
	}
	EnemyDrawnWidgets.Empty();
}
