// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThreadOfFateCharacter.generated.h"

UCLASS(config = Game)
class AThreadOfFateCharacter : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

public:
    AThreadOfFateCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;

protected:
    void OnResetVR();

    void MoveForward(float Value);

    void MoveRight(float Value);

    void TurnAtRate(float Rate);

    void LookUpAtRate(float Rate);

    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

    void Sprint();
    void StopSprinting();

    void StartHealing();

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void HealArmor(float Amount);

    /*UFUNCTION(BlueprintImplemetableEvent, Category = "Items")
    AActor* DetermineOverlappingItem();*/

    void StartDamage();

    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float Amount);

    void EquipItem();

    bool IsSprinting = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float PlayerHealth = 0.00f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    bool IsOverlappingItem = false;

    bool HasArmor = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float PlayerArmor = 0.00f;


protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};