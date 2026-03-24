// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "UObject/Object.h"
#include "ActionObject.generated.h"

/**
 *  That a simple action object, it does not have replication or RPC,
 *  that actions will work only in Standalone or Server.
 *  If your game has Multiplayer, use ActionReplicatedObject instead.
 *  
 *  Action has all standard input events:
 *  ActionStarted_Event
 *  ActionTriggered_Event
 *  ActionOngoing_Event
 *  ActionCanceled_Event
 *  ActionCompleted_Event
 */

class UPlayerActionComponent;

UCLASS(Abstract, Blueprintable, meta=(ShowWorldContextPin))
class PLAYERACTIONS_API UActionObject : public UObject
{
	GENERATED_BODY()

#pragma region UObject
public:
	virtual UWorld* GetWorld() const override;
	
	UFUNCTION(BlueprintPure, Category="Action|Object")
	float GetWorldDeltaSecond() const;

	UFUNCTION(BlueprintCallable, Category="Action|Object")
	AActor* SpawnActorByClass(UClass* Class, const FVector& Location, const FRotator& Rotation);
#pragma endregion 

#pragma region BlueprintEvents
protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Action")
	void ActionStarted_Event(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintImplementableEvent, Category="Action")
	void ActionTriggered_Event(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintImplementableEvent, Category="Action")
	void ActionOngoing_Event(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintImplementableEvent, Category="Action")
	void ActionCanceled_Event(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintImplementableEvent, Category="Action")
	void ActionCompleted_Event(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintImplementableEvent, Category="Action")
	void OnActionUnbind_Event();

#pragma endregion 

#pragma region UActionObject
public:
	UActionObject();

	uint32 GetID() const {return ActionId;}
	void SetID(const uint32 Id) {ActionId = Id;}

	static UActionObject* NewActionObject(UObject* Outer, TSubclassOf<UActionObject> Class);

	UFUNCTION(BlueprintPure, Category="Action")
	APlayerController* GetOwnerController() const;

	UFUNCTION(BlueprintPure, Category="Action")
	APawn* GetOwnerPawn() const;

	UFUNCTION(BlueprintPure, Category="Action")
	UPlayerActionComponent* GetActionComponent() const;
	
	virtual bool IsReplicated();

	virtual void ActionStarted(const FInputActionValue& ActionValue);

	virtual void ActionTriggered(const FInputActionValue& ActionValue);

	virtual void ActionOngoing(const FInputActionValue& ActionValue);

	virtual void ActionCanceled(const FInputActionValue& ActionValue);

	virtual void ActionCompleted(const FInputActionValue& ActionValue);

	virtual void OnActionUnbind();

private:
	uint32 ActionId;

#pragma endregion 
};
