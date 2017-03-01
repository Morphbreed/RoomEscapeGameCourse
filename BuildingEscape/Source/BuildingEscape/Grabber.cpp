// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();		
	FindPhysicsHandleComponent();
	SetupInputComponent();	
}

///Look for attached Physics Handle
void UGrabber::FindPhysicsHandleComponent()
{	
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
		//PhysicsHandle found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Physicshandle found in %s"), *GetOwner()->GetName());
	};
}

///Look for attached Input component
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("Input Component found"));
		// BInd the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No inputcomponent found in %s"), *GetOwner()->GetName());
	};
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("grab pressed"));

	// LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	// if we hit something then attach a physics handle
	if (ActorHit)
	{
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true //allow rotation
		);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("grab released"));

	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	// get the player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewRotation
	);

	FVector LinceTraceEnd = PlayerViewPointLocation + (PlayerViewRotation.Vector() * Reach);

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LinceTraceEnd);
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	// get the player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewRotation
	);

	FVector LinceTraceEnd = PlayerViewPointLocation + (PlayerViewRotation.Vector() * Reach);

	/// setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	/// ray-cast out to reach distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LinceTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	/// see what we hit
	//Hit.GetActor()->GetName();
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorHit->GetName()));
	}

	return Hit;
}