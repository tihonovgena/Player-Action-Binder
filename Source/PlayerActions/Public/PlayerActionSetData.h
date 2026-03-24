// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerActionSetData.generated.h"

class UActionObject;
class UInputAction;
class UInputMappingContext;

/**
 *  That data stores information about inputs and actions.
 *  You should put InputMappingContext and fill ActionInputs and ActionObjectClasses in ActionAssociation
 */

UCLASS()
class PLAYERACTIONS_API UPlayerActionSetData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPlayerActionSetData();
	
	// Uniq set ID
	uint32 GetSetID() const {return SetID;}
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	// InputMappingContext priority
	UPROPERTY(EditAnywhere, Category="Input")
	uint32 Priority;

	// Pair of InputAction and ActionClass
	UPROPERTY(EditAnywhere, Category="Input")
	TMap<TObjectPtr<UInputAction>, TSubclassOf<UActionObject>> InputActionMapping;

private:
	uint32 SetID = 0;
};
