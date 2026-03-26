// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionRoguelike.h"
#include "ActionSystem/RogueAction.h"
#include "RogueAction_Dash.generated.h"

class UNiagaraSystem;

/**
 * Dash Action - Moves the character forward at high speed.
 * 
 * Uses a sweep trace to prevent clipping through thin walls.
 * Replaces the Blueprint implementation that used SetActorLocation
 * with only endpoint collision checking (GAME-1 fix).
 */
UCLASS()
class ACTIONROGUELIKE_API URogueAction_Dash : public URogueAction
{
	GENERATED_BODY()

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

protected:

	/* Distance to dash forward (in Unreal units) */
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDistance = 1000.f;

	/* Duration of the dash for cosmetic interpolation (does not affect travel distance) */
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.2f;

	/* Optional particle effect during dash */
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Effects")
	TObjectPtr<UNiagaraSystem> DashEffect;

	/* Optional camera shake on dash start */
	UPROPERTY(EditDefaultsOnly, Category = "Dash|Effects")
	TSubclassOf<UCameraShakeBase> DashCameraShake;

	/**
	 * Performs a safe sweep-based movement that stops at wall collisions.
	 * This is the core fix for GAME-1: instead of teleporting to the endpoint
	 * and only checking collision there, we sweep the character's full capsule
	 * along the dash path so thin walls are detected.
	 *
	 * @param Character  The character performing the dash
	 * @param DashDirection  Normalized direction to dash
	 * @return The actual location after the sweep (may be shorter than DashDistance if blocked)
	 */
	FVector PerformSweepDash(ACharacter* Character, const FVector& DashDirection) const;
};
