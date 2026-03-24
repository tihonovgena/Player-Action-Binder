// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActionObject.h"
#include "ActionReplicatedObject.generated.h"

/**
 *  That a simple action object, that has replication and RPC,
 *  those actions will work with any Network modes and Standalone.
 *  If your game doesn't have Multiplayer, use ActionObject instead.
 *  
 *  Action has all standard input events:
 *  ActionStarted_Event
 *  ActionTriggered_Event
 *  ActionOngoing_Event
 *  ActionCanceled_Event
 *  ActionCompleted_Event
 */

UCLASS(Abstract)
class PLAYERACTIONS_API UActionReplicatedObject : public UActionObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;
	virtual bool IsReplicated() override;;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool IsSupportedForNetworking() const override;
};
