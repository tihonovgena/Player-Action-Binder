// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.


#include "PawnActionSetComponent.h"

#include "PlayerActionComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UPawnActionSetComponent::UPawnActionSetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPawnActionSetComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetPawnOwner(), TEXT("The component must have a pawn as its owner"));

	APawn* Owner = Cast<APawn>(GetOwner());
	if (Owner && Owner->HasAuthority())
	{
		// Binds on changing controllers
		Owner->ReceiveControllerChangedDelegate.AddDynamic(this, &UPawnActionSetComponent::OnControllerChanged);
		
		// Adds input set if an owner pawn already has a controller
		const APlayerController* Controller = Cast<APlayerController>(Owner->Controller);
		if (Controller)
		{
			AddInputSetsToController(Controller);
		}
		
	}
}

APawn* UPawnActionSetComponent::GetPawnOwner() const
{
	return Cast<APawn>(GetOwner());
}

void UPawnActionSetComponent::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	if(const APlayerController* OldPlayerController = Cast<APlayerController>(OldController))
	{
		RemoveInputSetsFromController(OldPlayerController);
	}

	if (const APlayerController* NewPlayerController = Cast<APlayerController>(NewController))
	{
		AddInputSetsToController(NewPlayerController);
	}
}

void UPawnActionSetComponent::AddInputSetsToController(const AController* Controller)
{
	if (Controller)
	{
		UActorComponent* ActorComponent = Controller->GetComponentByClass(UPlayerActionComponent::StaticClass());
		UPlayerActionComponent* ActionSystemComponent = Cast<UPlayerActionComponent>(ActorComponent);
		if (ActionSystemComponent)
		{
			for(const auto& Action : ActionInputSet)
			{
				ActionSystemComponent->AddInputSet(Action.LoadSynchronous());
			}
		}
	}
}

void UPawnActionSetComponent::RemoveInputSetsFromController(const AController* Controller)
{
	if (Controller && Controller->HasAuthority())
	{
		UActorComponent* ActorComponent = Controller->GetComponentByClass(UPlayerActionComponent::StaticClass());
		UPlayerActionComponent* ActionSystemComponent = Cast<UPlayerActionComponent>(ActorComponent);
		for(const auto& Action : ActionInputSet)
		{
			ActionSystemComponent->RemoveInputSet(Action.LoadSynchronous());
		}
		
	}
}


