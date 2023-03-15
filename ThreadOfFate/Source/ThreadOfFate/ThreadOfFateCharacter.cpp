// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreadOfFateCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

AThreadOfFateCharacter::AThreadOfFateCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;            // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;       // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom
                                                                                // adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false;                              // Camera does not rotate relative to arm

    IsSprinting = false;

    PlayerHealth = 1.00f;

    IsOverlappingItem = false;

    PlayerArmor = 1.00f;
    HasArmor = true;

    IsZoomed = false;
}

void AThreadOfFateCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AThreadOfFateCharacter::Sprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AThreadOfFateCharacter::StopSprinting);

    PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &AThreadOfFateCharacter::StartHealing);
    PlayerInputComponent->BindAction("Damage", IE_Released, this, &AThreadOfFateCharacter::StartDamage);

    PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AThreadOfFateCharacter::EquipItem);

    PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AThreadOfFateCharacter::ZoomIn);
    PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AThreadOfFateCharacter::StopZoom);

    PlayerInputComponent->BindAxis("MoveForward", this, &AThreadOfFateCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AThreadOfFateCharacter::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AThreadOfFateCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AThreadOfFateCharacter::LookUpAtRate);

    PlayerInputComponent->BindTouch(IE_Pressed, this, &AThreadOfFateCharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AThreadOfFateCharacter::TouchStopped);

    PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AThreadOfFateCharacter::OnResetVR);
}

void AThreadOfFateCharacter::OnResetVR()
{
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AThreadOfFateCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
    Jump();
}

void AThreadOfFateCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
    StopJumping();
}

void AThreadOfFateCharacter::TurnAtRate(float Rate)
{
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AThreadOfFateCharacter::LookUpAtRate(float Rate)
{
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AThreadOfFateCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AThreadOfFateCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AThreadOfFateCharacter::Sprint()
{
    IsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = 1500.0f;
}

void AThreadOfFateCharacter::StopSprinting()
{
    IsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AThreadOfFateCharacter::StartHealing()
{
    Heal(0.02f);
}

void AThreadOfFateCharacter::Heal(float Amount)
{
    PlayerHealth += Amount;
    if (PlayerHealth > 1.00f)
    {
        PlayerHealth = 1.00f;
    }
}

void AThreadOfFateCharacter::StartDamage()
{
    TakeDamage(0.02f);
}

void AThreadOfFateCharacter::TakeDamage(float Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("We are taking damage for %f points."), Amount);

    if (HasArmor)
    {
        PlayerArmor -= Amount;
        if (PlayerArmor < 0.00f)
        {
            HasArmor = false;
            PlayerHealth += PlayerArmor;
            PlayerArmor = 0.00f;
        }
    }
    else
    {
        PlayerHealth -= Amount;
        if (PlayerHealth < 0.00f)
        {
            PlayerHealth = 0.00f;
        }
    }
}

void AThreadOfFateCharacter::EquipItem()
{
    if (IsOverlappingItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("We picked up an item"));
    }
}

void AThreadOfFateCharacter::HealArmor(float Amount)
{
    PlayerArmor += Amount;
    HasArmor = true;

    if (PlayerArmor > 1.00f)
    {
        PlayerArmor = 1.00f;
    }
}

void AThreadOfFateCharacter::ZoomIn()
{
    if (auto ThirdPersonCamera = GetCameraBoom())
    {
        ThirdPersonCamera->TargetArmLength = 150.0f;
        ThirdPersonCamera->TargetOffset = FVector(0.0f, 20.0f, 70.0f);

        if (auto characterMovement = GetCharacterMovement())
        {
            characterMovement->MaxWalkSpeed = 300.0f;
        }

        IsZoomed = true;
    }
}

void AThreadOfFateCharacter::StopZoom() 
{
    if (auto ThirdPersonCamera = GetCameraBoom())
    {
        ThirdPersonCamera->TargetArmLength = 300.0f;
        ThirdPersonCamera->TargetOffset = FVector(0.0f, 0.0f, 0.0f);

        if (auto characterMovement = GetCharacterMovement())
        {
            characterMovement->MaxWalkSpeed = 600.0f;
        }

        IsZoomed = false;
    }
}
