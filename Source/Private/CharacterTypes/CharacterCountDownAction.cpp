// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTypes/CharacterCountDownAction.h"

void FCharacterCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	
	if (bNeedToCancel)
	{
		CountDownOutput = ECharacterCountdownActionOutput::Canceled;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}
	
	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		CountDownOutput = ECharacterCountdownActionOutput::Completed;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}
	
	if (ElapsedInterval < UpdateInterval)
		ElapsedInterval += Response.ElapsedTime();
	else
	{
		ElapsedTimeSinceStart += UpdateInterval > 0.f? UpdateInterval : Response.ElapsedTime();
		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;
		CountDownOutput = ECharacterCountdownActionOutput::Updated;
		
		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
		
		ElapsedInterval = 0.f;
	}
}

void FCharacterCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
