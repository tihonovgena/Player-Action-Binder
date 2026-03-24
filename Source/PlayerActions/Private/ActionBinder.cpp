// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.


#include "ActionBinder.h"

#include "PlayerActionComponent.h"
#include "InputAction.h"
#include "Actions/ActionObject.h"

UActionBinder* UActionBinder::NewActionBinder(UObject* Outer)
{
	return NewObject<UActionBinder>(Outer);
}

void UActionBinder::TriggeredCallback(const FInputActionInstance& Instance)
{
	if (ActionObject)
	{
		ActionObject->ActionTriggered(Instance.GetValue());
	}
}

void UActionBinder::OngoingCallback(const FInputActionInstance& Instance)
{
	if (ActionObject)
	{
		ActionObject->ActionOngoing(Instance.GetValue());
	}
}

void UActionBinder::CanceledCallback(const FInputActionInstance& Instance)
{
	if (ActionObject)
	{
		ActionObject->ActionCanceled(Instance.GetValue());
	}
}

void UActionBinder::StartedCallback(const FInputActionInstance& Instance)
{
	if (ActionObject)
	{
		ActionObject->ActionStarted(Instance.GetValue());
	}
}

void UActionBinder::CompletedCallback(const FInputActionInstance& Instance)
{
	if (ActionObject)
	{
		ActionObject->ActionCompleted(Instance.GetValue());
	}
}

void UActionBinder::CallActionUnbind()
{
	if (ActionObject)
	{
		ActionObject->OnActionUnbind();
	}
}
