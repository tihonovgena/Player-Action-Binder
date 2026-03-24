// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.


#include "Actions/ActionReplicatedObject.h"

#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/NetDriver.h"
#include "GameFramework/PlayerController.h"

UWorld* UActionReplicatedObject::GetWorld() const
{
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
}

bool UActionReplicatedObject::IsReplicated()
{
	return true;
}

void UActionReplicatedObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

bool UActionReplicatedObject::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	APlayerController* PlayerController = GetOwnerController();
	UNetDriver* NetDriver = PlayerController->GetNetDriver();
	if (NetDriver)
	{
		NetDriver->ProcessRemoteFunction(PlayerController, Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}

int32 UActionReplicatedObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UActionReplicatedObject::IsSupportedForNetworking() const
{
	return true;
}
