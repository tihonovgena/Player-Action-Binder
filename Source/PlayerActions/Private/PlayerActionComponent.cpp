// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.


#include "PlayerActionComponent.h"

#include "ActionBinder.h"
#include "PlayerActionSetData.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Actions/ActionObject.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Net/UnrealNetwork.h"

TArray<uint32> FActionBinderArray::GetBindHandles()
{
	TArray<uint32> Array;
	for(const auto& Binder : ActionBinders)
	{
		if (Binder)
		{
			Array.Append(Binder->GetBindHandles());
		}
	}

	return Array;
}

bool FActionBinderArray::WasAnyBind(const UClass* ActionClass)
{
	for (const auto& Item : ActionBinders)
	{
		if (Item->WasActionBind(ActionClass))
		{
			return true;
		}
	}

	return false;
}

UPlayerActionComponent::UPlayerActionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
	bReplicateUsingRegisteredSubObjectList = true;
}

void UPlayerActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwnerPlayer(), TEXT("The component must have a player controller as its owner"));

	APlayerController* PlayerController = GetOwnerPlayer();
	// If client ask about init
	if (!PlayerController->HasAuthority())
	{
		InitRequest_Server();
	}
	// Else if authority and local init directly
	else if (PlayerController->IsLocalController())
	{
		InitActionComponent();
	}
	
}

void UPlayerActionComponent::InitActionComponent()
{
	for(const auto& Set: ActionInputSet)
	{
		if (!Set.IsNull())
		{
			AddInputSet(Set.LoadSynchronous());
		}
	}
}

APlayerController* UPlayerActionComponent::GetOwnerPlayer() const
{;
	return Cast<APlayerController>(GetOwner());
}

void UPlayerActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UPlayerActionComponent, ReplicatedInputActionPairs);
}

void UPlayerActionComponent::InitRequest_Server_Implementation()
{
	InitActionComponent();
}

UInputComponent* UPlayerActionComponent::GetOwnerInputComponent() const
{
	if (const APlayerController* PlayerController = GetOwnerPlayer())
	{
		return PlayerController->InputComponent;
	}

	return nullptr;
}

void UPlayerActionComponent::AddInputSet(const UPlayerActionSetData* InputSet)
{
	if(!InputSet)
	{
		UE_LOG(ActionComponent, Error, TEXT("Can't add input set. UActionInputSetData is null"));
		return;
	};

	const APlayerController* PlayerController = GetOwnerPlayer();
	// If PlayerController is local authority controller
	if (PlayerController->IsLocalPlayerController())
	{
		// Add mapping context directly
		AddInputMappingContext(InputSet->InputMapping, InputSet->Priority);
		
		TArray<FInputActionPair> Pairs;
		MakePlayerInputActionPairs(InputSet, Pairs);

		// Bind inputs directly
		BindPlayerInputActionPairs(Pairs);
	}
	else
	{
		// Add mapping context to OwnerClient
		AddInputMappingContext_Client(InputSet->InputMapping, InputSet->Priority);
		
		TArray<FInputActionPair> Pairs;
		MakePlayerInputActionPairs(InputSet, Pairs);

		// Put pairs to replicated array, it will bind on rep notify
		ReplicatedInputActionPairs.Append(Pairs);
	}
	
}

void UPlayerActionComponent::RemoveInputSet(const UPlayerActionSetData* InputSet)
{
	if(!InputSet)
	{
		UE_LOG(ActionComponent, Error, TEXT("Can't remove input set. UActionInputSetData is null"));
		return;
	};

	const APlayerController* PlayerController = GetOwnerPlayer();
	// If PlayerController is local authority controller
	if (PlayerController->IsLocalPlayerController())
	{
		// Remove mapping context directly
		RemoveInputMappingContext(InputSet->InputMapping);

		// Unbind input set directly
		UnbindInputSet(InputSet->GetSetID());
	}
	else
	{
		// Remove mapping context to OwnerClient
		RemoveInputMappingContext_Client(InputSet->InputMapping);

		// Unbind input set to OwnerClient
		UnbindInputSet_Client(InputSet->GetSetID());

		// Remove InputActionPairs with InputSetID
		for (int32 Index = ReplicatedInputActionPairs.Num() - 1; Index >= 0; --Index)
		{
			if (ReplicatedInputActionPairs[Index].ActionSetID == InputSet->GetSetID())
			{
				ReplicatedInputActionPairs.RemoveAt(Index);
			}
		}
		
	}
	
}

void UPlayerActionComponent::MakePlayerInputActionPairs(const UPlayerActionSetData* InputSet,
	TArray<FInputActionPair>& OutParam)
{
	for(const auto& Pair : InputSet->InputActionMapping)
	{
		// Create action object
		UActionObject* ActionObject = UActionObject::NewActionObject(this, Pair.Value);
		
		// Create pair
		FInputActionPair InputActionPair(Pair.Key, ActionObject, InputSet->GetSetID());

		// Add pair to outer array
		OutParam.Add(InputActionPair);

		// Setup for replication if it is replicated
		if (GetIsReplicated() && ActionObject->IsReplicated())
		{
			AddReplicatedSubObject(ActionObject);
		}
	}
}

void UPlayerActionComponent::AddInputMappingContext_Client_Implementation(UInputMappingContext* MappingContext, const int32 Priority)
{
	AddInputMappingContext(MappingContext, Priority);
}

void UPlayerActionComponent::AddInputMappingContext(const UInputMappingContext* MappingContext, const int32 Priority) const
{
	const APlayerController* PlayerController = GetOwnerPlayer();
	if(!MappingContext && !PlayerController)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't add an input set mapping context, "
										"there is no MappingContext or PlayerController"));
		return;
	};
	
	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if(!LocalPlayer)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't add an input set mapping context, "
										"there is no LocalPlayer"));
		return;
	};

	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't add an input set mapping context, "
										"there is no EnhancedInputLocalPlayerSubsystem"));
		return;
	}

	InputSystem->AddMappingContext(MappingContext, Priority);
	UE_LOG(ActionComponent, Warning, TEXT("Added mapping context of %s"), *MappingContext->GetName());
}

void UPlayerActionComponent::RemoveInputMappingContext(const UInputMappingContext* MappingContext) const
{
	const APlayerController* PlayerController = GetOwnerPlayer();
	if(!MappingContext && !PlayerController)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't remove an input set mapping context, "
										"there is no MappingContext or PlayerController"));
		return;
	};
	
	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if(!LocalPlayer)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't remove an input set mapping context, "
										"there is no LocalPlayer"));
		return;
	};

	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't remove an input set mapping context, "
										"there is no EnhancedInputLocalPlayerSubsystem"));
		return;
	}

	InputSystem->RemoveMappingContext(MappingContext);
	UE_LOG(ActionComponent, Warning, TEXT("Removed mapping context of %s"), *MappingContext->GetName());
}

void UPlayerActionComponent::RemoveInputMappingContext_Client_Implementation(UInputMappingContext* MappingContext)
{
	RemoveInputMappingContext(MappingContext);
}

void UPlayerActionComponent::UnbindInputSet(const uint32 SetID)
{
	const APlayerController* PlayerController = GetOwnerPlayer();
	if(!PlayerController) return;
	
	// Unbind inputs
	FActionBinderArray* ActionBindersArray = ActionBindersMap.Find(SetID);
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	if (ActionBindersArray && Input)
	{
		for(const auto& Binder : ActionBindersArray->GetBinders())
		{
			
			// Unbind all ActionInputs
			for(const auto& Handle : Binder->GetBindHandles())
			{
				Input->RemoveBindingByHandle(Handle);
			}
			
			FInputActionInstance InputActionInstance;
			Binder->CallActionUnbind();
	
		}
		
		// Clear all ActionBinders
		ActionBindersArray->ClearActionBinders();
	}

	// Remove InputSet from the map
	ActionBindersMap.Remove(SetID);
}

void UPlayerActionComponent::UnbindInputSet_Client_Implementation(const uint32 SetID)
{
	UnbindInputSet(SetID);
}

void UPlayerActionComponent::BindPlayerInputActionPairs(TArray<FInputActionPair>& Pairs)
{
	for (auto& Pair : Pairs)
	{
		// Tries find existed ActionBinderArray
		if (FActionBinderArray* BinderArray = ActionBindersMap.Find(Pair.ActionSetID))
		{
			// Check if we don't have bind of that action 
			if (!BinderArray->WasAnyBind(Pair.ActionObject->GetClass()))
			{
				UActionBinder* ActionBinder = BindAction(Pair.InputAction, Pair.ActionObject);
				BinderArray->AddActionBinder(ActionBinder);
			}
		}
		// Else create new BinderArray and adds Binder to it
		else
		{
			FActionBinderArray NewBinderArray;
			UActionBinder* ActionBinder = BindAction(Pair.InputAction, Pair.ActionObject);
			NewBinderArray.AddActionBinder(ActionBinder);
			ActionBindersMap.Add(Pair.ActionSetID, NewBinderArray);
		}
	}
}

UActionBinder* UPlayerActionComponent::BindAction(const UInputAction* InputAction, UActionObject* ActionObject)
{
	if (!InputAction && !ActionObject)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't proceed binding action, InputAction or Action Object is null"));
		return nullptr;
	}
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetOwnerInputComponent());
	if (!EnhancedInputComponent)
	{
		UE_LOG(ActionComponent, Warning, TEXT("Can't proceed binding action, EnhancedInputComponent is null"));
		return nullptr;
	};

	if (UActionBinder* ActionBinder = UActionBinder::NewActionBinder(this))
	{
		ActionBinder->SetActionObject(ActionObject);

		const uint32 StartedHandle = EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, ActionBinder,
				&UActionBinder::StartedCallback).GetHandle();
		ActionBinder->AddBindHandle(StartedHandle);

		const uint32 OngoingHandle = EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Ongoing, ActionBinder,
			&UActionBinder::OngoingCallback).GetHandle();
		ActionBinder->AddBindHandle(OngoingHandle);
			
		const uint32 TriggeredHandle = EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, ActionBinder,
			&UActionBinder::TriggeredCallback).GetHandle();
		ActionBinder->AddBindHandle(TriggeredHandle);

		const uint32 CanceledHandle = EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Canceled, ActionBinder,
			&UActionBinder::CanceledCallback).GetHandle();
		ActionBinder->AddBindHandle(CanceledHandle);

		const uint32 CompletedHandle = EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, ActionBinder,
			&UActionBinder::CompletedCallback).GetHandle();
		ActionBinder->AddBindHandle(CompletedHandle);

		UE_LOG(ActionComponent, Log, TEXT("Bind action: %s"), *GetNameSafe(InputAction));
		return ActionBinder;
	}
	
	UE_LOG(ActionComponent, Warning, TEXT("Can't proceed binding action, ActionBinder is null"));
	return nullptr;
}

void UPlayerActionComponent::OnRep_InputActionPairs()
{
	BindPlayerInputActionPairs(ReplicatedInputActionPairs);
}
