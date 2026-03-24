// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerActionSetData.h"
#include "Components/ActorComponent.h"
#include "PlayerActionComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UInputComponent;
class UActionBinder;
class UPlayerActionSetData;
class UActionObject;

USTRUCT(Blueprintable)
struct FActionBinderArray
{
	GENERATED_BODY()
	FActionBinderArray() = default;
	~FActionBinderArray() = default;

	TArray<uint32> GetBindHandles();
	
	void AddActionBinder(UActionBinder* Binder)
	{
		ActionBinders.Add(Binder);
	};

	void ClearActionBinders()
	{
		ActionBinders.Empty();
	}

	bool WasAnyBind(const UClass* ActionClass);

	TArray<TObjectPtr<UActionBinder>>& GetBinders(){return ActionBinders;}

private:
	UPROPERTY()
	TArray<TObjectPtr<UActionBinder>> ActionBinders;
	
};

USTRUCT()
struct FInputActionPair
{
	GENERATED_BODY()

	FInputActionPair()
		: InputAction(nullptr), ActionObject(nullptr)
	{
		ActionSetID = 0;
	}
	
	FInputActionPair(UInputAction* Input, UActionObject* Action, const uint32 SetID)
		: InputAction(Input), ActionObject(Action), ActionSetID(SetID)
	{
	}

	bool operator==(const FInputActionPair& Other) const
	{
		return InputAction == Other.InputAction && ActionObject == Other.ActionObject;
	}

	UPROPERTY()
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY()
	TObjectPtr<UActionObject> ActionObject;

	UPROPERTY()
	uint32 ActionSetID;
};


DECLARE_LOG_CATEGORY_CLASS(ActionComponent, Log, All);

/*
 * Provides simple way to associate Actions with InputActions.
 * Component should be located in PlayerController as a safety place for run available actions.
 * That component takes PlayerActionSetData - information about inputs and available actions.
 * Just setup PlayerActionSetData in your PlayerController blueprint.
 * For extend actions for different Pawns, you can use ActionInputSetComponent that also includes PlayerActionSetData.
*/

UCLASS(ClassGroup=(Action), meta=(BlueprintSpawnableComponent, DisplayName = "Player Action Component"))
class PLAYERACTIONS_API UPlayerActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerActionComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	void InitActionComponent();

	UFUNCTION(BlueprintPure, Category="PlayerActions|Misc")
	APlayerController* GetOwnerPlayer() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION(Server, Reliable)
	void InitRequest_Server();
	
	UInputComponent* GetOwnerInputComponent() const;
	
#pragma region Inputs
public:
	UFUNCTION(BlueprintCallable, Category="PlayerActions")
	void AddInputSet(const UPlayerActionSetData* InputSet);

	UFUNCTION(BlueprintCallable, Category="PlayerActions")
	void RemoveInputSet(const UPlayerActionSetData* InputSet);

private:
	// ActionInputSet that will be available for owner player controller
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TArray<TSoftObjectPtr<UPlayerActionSetData>> ActionInputSet;

	/**
	 * Using @param InputSet creates TArray<FPlayerInputActionPair> and put it in @param OutParam
	 */
	void MakePlayerInputActionPairs(const UPlayerActionSetData* InputSet, TArray<FInputActionPair>& OutParam);

	void AddInputMappingContext(const UInputMappingContext* MappingContext, int32 Priority) const;
	
	UFUNCTION(Client, Reliable)
	void AddInputMappingContext_Client(UInputMappingContext* MappingContext, int32 Priority);
	
	void RemoveInputMappingContext(const UInputMappingContext* MappingContext) const;

	UFUNCTION(Client, Reliable)
	void RemoveInputMappingContext_Client(UInputMappingContext* MappingContext);
	
	void UnbindInputSet(const uint32 SetID);

	UFUNCTION(Client, Reliable)
	void UnbindInputSet_Client(const uint32 SetID);

	void BindPlayerInputActionPairs(TArray<FInputActionPair>& Pairs);
	
	UActionBinder* BindAction(const UInputAction* InputAction, UActionObject* ActionObject);
	
	// int32 - Uniq ID of ActionInputSetData,  UActionBinder - Binder which was created during binding InputSet
	UPROPERTY()
	TMap<int32, FActionBinderArray> ActionBindersMap;
	
	// Array of available inputs and actions for player controller
	UPROPERTY(ReplicatedUsing=OnRep_InputActionPairs)
	TArray<FInputActionPair> ReplicatedInputActionPairs;
	
	UFUNCTION()
	void OnRep_InputActionPairs();
	
#pragma endregion 
};
