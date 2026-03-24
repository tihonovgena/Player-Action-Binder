// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ActionBinder.generated.h"

class UInputAction;
class UPlayerActionComponent;
class UActionObject;
struct FInputActionInstance;

/**
 *  Make connection between ActionInput and ActionObject.
 *  You should BindAction using that object.
 *  Inputs will be bind in ActionSystemComponent.
 */

UCLASS()
class PLAYERACTIONS_API UActionBinder : public UObject
{
	GENERATED_BODY()

public:
	static UActionBinder* NewActionBinder(UObject* Outer);

	void AddBindHandle(const uint32 Handle){ActionInputHandles.Add(Handle);}
	
	const TArray<uint32>& GetBindHandles(){return ActionInputHandles;}

	UFUNCTION()
	void StartedCallback(const FInputActionInstance& Instance);
	
	UFUNCTION()
	void TriggeredCallback(const FInputActionInstance& Instance);

	UFUNCTION()
	void OngoingCallback(const FInputActionInstance& Instance);

	UFUNCTION()
	void CanceledCallback(const FInputActionInstance& Instance);

	UFUNCTION()
	void CompletedCallback(const FInputActionInstance& Instance);

	UFUNCTION()
	void CallActionUnbind();

	bool WasActionBind(const UClass* Class) const {return Class == ActionObject.GetClass();}
	
	void SetActionObject(UActionObject* Action){ActionObject = Action;}

private:
	UPROPERTY()
	TObjectPtr<UActionObject> ActionObject;
	
	TArray<uint32> ActionInputHandles;

};
