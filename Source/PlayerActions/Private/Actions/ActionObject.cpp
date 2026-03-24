// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.


#include "Actions/ActionObject.h"

#include "Engine/Engine.h"
#include "PlayerActionComponent.h"
#include "GameFramework/PlayerController.h"

UActionObject::UActionObject()
{
	ActionId = 0;
}

UActionObject* UActionObject::NewActionObject(UObject* Outer, TSubclassOf<UActionObject> Class)
{
	if (Class)
	{
		return NewObject<UActionObject>(Outer, Class);
	}

	return nullptr;
}

bool UActionObject::IsReplicated()
{
	return false;
}

APlayerController* UActionObject::GetOwnerController() const
{
	return GetTypedOuter<APlayerController>();
}

APawn* UActionObject::GetOwnerPawn() const
{
	if (GetOwnerController())
	{
		return GetOwnerController()->GetPawn();
	}
	return nullptr;
}

UPlayerActionComponent* UActionObject::GetActionComponent() const
{
	return GetTypedOuter<UPlayerActionComponent>();
}

float UActionObject::GetWorldDeltaSecond() const
{
	if (GetWorld())
	{
		return GetWorld()->GetDeltaSeconds();
	}

	return 0.f;
}

AActor* UActionObject::SpawnActorByClass(UClass* Class, const FVector& Location, const FRotator& Rotation)
{
	if (GetWorld())
	{
		return GetWorld()->SpawnActor(Class, &Location, &Rotation);
	}

	return nullptr;
}

void UActionObject::ActionStarted(const FInputActionValue& ActionValue)
{
	ActionStarted_Event(ActionValue);
}

void UActionObject::ActionTriggered(const FInputActionValue& ActionValue)
{
	ActionTriggered_Event(ActionValue);
}

void UActionObject::ActionOngoing(const FInputActionValue& ActionValue)
{
	ActionOngoing_Event(ActionValue);
}

void UActionObject::ActionCanceled(const FInputActionValue& ActionValue)
{
	ActionCanceled_Event(ActionValue);
}

void UActionObject::ActionCompleted(const FInputActionValue& ActionValue)
{
	ActionCompleted_Event(ActionValue);
}

void UActionObject::OnActionUnbind()
{
	OnActionUnbind_Event();
}

UWorld* UActionObject::GetWorld() const
{
	if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	
	return nullptr;
}
