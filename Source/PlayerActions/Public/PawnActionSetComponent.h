// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnActionSetComponent.generated.h"

/*
 * The component for Pawn, it will add new ActionSetData to PlayerController when it will Possess an owner pawn
 * And it will remove PlayerActionSetData from PlayerController when it will Unpossess an owner pawn.
 * Just add that component to Pawn and set up PlayerActionSetData you want use in that Pawn.
 */

class UPlayerActionSetData;
class AController;

UCLASS(ClassGroup=(Action), meta=(BlueprintSpawnableComponent))
class PLAYERACTIONS_API UPawnActionSetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPawnActionSetComponent();

	virtual void BeginPlay() override;

protected:
	APawn* GetPawnOwner() const;

private:
	UFUNCTION()
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	void AddInputSetsToController(const AController* Controller);
	void RemoveInputSetsFromController(const AController* Controller);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TArray<TSoftObjectPtr<UPlayerActionSetData>> ActionInputSet;
};
