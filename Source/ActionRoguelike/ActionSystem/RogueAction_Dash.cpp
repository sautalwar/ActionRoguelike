// Fill out your copyright notice in the Description page of Project Settings.

#include "RogueAction_Dash.h"
#include "ActionRoguelike.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RogueAction_Dash)


void URogueAction_Dash::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = CastChecked<ACharacter>(Instigator);

	// Dash in the direction the character is facing (controller rotation)
	const FVector DashDirection = Character->GetActorForwardVector();

	// --- GAME-1 FIX ---
	// Previously this was: Character->SetActorLocation(Character->GetActorLocation() + DashDirection * DashDistance)
	// That single-point check missed thin walls entirely. Now we sweep the full capsule.
	const FVector FinalLocation = PerformSweepDash(Character, DashDirection);

	// Play dash VFX at the starting position
	if (DashEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DashEffect,
			Character->GetActorLocation(),
			DashDirection.Rotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease
		);
	}

	// Camera shake for game feel
	if (DashCameraShake)
	{
		APlayerController* PC = Cast<APlayerController>(Character->GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(DashCameraShake);
		}
	}

	// Finished — stop the action so cooldown begins
	StopAction(Instigator);
}


FVector URogueAction_Dash::PerformSweepDash(ACharacter* Character, const FVector& DashDirection) const
{
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	UCapsuleComponent* Capsule = Character->GetCapsuleComponent();

	if (!ensure(MoveComp && Capsule))
	{
		return Character->GetActorLocation();
	}

	const FVector StartLocation = Character->GetActorLocation();
	const FVector DesiredEndLocation = StartLocation + (DashDirection * DashDistance);

	// --- THE FIX: Sweep the full capsule along the dash path ---
	// This detects ANY geometry between start and end, including thin walls
	// that a simple endpoint check would miss.
	FHitResult SweepHit;
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(
		Capsule->GetScaledCapsuleRadius(),
		Capsule->GetScaledCapsuleHalfHeight()
	);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	const bool bHitSomething = GetWorld()->SweepSingleByChannel(
		SweepHit,
		StartLocation,
		DesiredEndLocation,
		FQuat::Identity,
		ECC_Pawn,
		CapsuleShape,
		QueryParams
	);

	FVector FinalLocation;
	if (bHitSomething)
	{
		// Stop just before the wall — pull back by a small margin to prevent interpenetration
		constexpr float WallMargin = 2.0f;
		FinalLocation = SweepHit.Location + (SweepHit.Normal * WallMargin);

		UE_LOG(LogActionRoguelike, Log,
			TEXT("Dash blocked by %s at distance %.1f / %.1f"),
			*GetNameSafe(SweepHit.GetActor()),
			FVector::Dist(StartLocation, FinalLocation),
			DashDistance
		);
	}
	else
	{
		// Nothing blocking — dash the full distance
		FinalLocation = DesiredEndLocation;
	}

	// Use SafeMoveUpdatedComponent for authoritative movement that respects replication
	const FVector MoveDelta = FinalLocation - StartLocation;
	MoveComp->SafeMoveUpdatedComponent(
		MoveDelta,
		Character->GetActorRotation(),
		true,  // bSweep — double-check with the movement component's own sweep
		SweepHit
	);

	return Character->GetActorLocation();
}
