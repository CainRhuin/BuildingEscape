// Copyright Mike Hill 2021

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle Component Found On: %s!"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if (HitResult.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetLineTraceEnd()
		);
	}
}

void UGrabber::Release()
{
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}	
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit, 
		GetPlayerWorldPos(), 
		GetLineTraceEnd(), 
		FCollisionObjectQueryParams(ECC_PhysicsBody), 
		TraceParams
	);
	return Hit;
}

FVector UGrabber::GetLineTraceEnd() const
{
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewpointLocation, OUT PlayerViewpointRotation);
	
	return PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayerWorldPos() const
{
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewpointLocation, OUT PlayerViewpointRotation);
	
	return PlayerViewpointLocation;
}
