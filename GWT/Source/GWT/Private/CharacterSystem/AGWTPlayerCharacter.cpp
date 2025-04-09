// GWTPlayerCharacter.cpp
// Implementation of the player character

#include "GWTPlayerCharacter.h"
#include "GWTInventory.h"
#include "GWTGrimoire.h"
#include "GWTPlayerProgression.h"
#include "GWTSpell.h"
#include "GWTItem.h"
#include "GWTEquipment.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGWTPlayerCharacter::AGWTPlayerCharacter()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    
    // Create camera boom (pulls toward the player if there's a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller
    
    // Create follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // Camera doesn't rotate relative to arm
    
    // Set default player stats - higher than base character
    MaxHealth = 150.0f;
    CurrentHealth = MaxHealth;
    MaxMana = 200.0f;
    CurrentMana = MaxMana;
    ManaRegenRate = 8.0f;
    MovementSpeed = 750.0f;
    
    // Configure character movement
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->bOrientRotationToMovement = true; // Character moves in the direction of input...
        MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
        MoveComp->JumpZVelocity = 600.f;
        MoveComp->AirControl = 0.2f;
        MoveComp->MaxWalkSpeed = MovementSpeed;
    }
    
    // Set spell casting properties
    CastingRange = 1500.0f;
    bIsCasting = false;
    CastingSpeed = 1.0f;
    ActiveSpellIndex = 0;
    
    // Create player systems
    Inventory = nullptr; // Created in BeginPlay
    Grimoire = nullptr; // Created in BeginPlay
    Progression = nullptr; // Created in BeginPlay
    
    // Log initialization
    UE_LOG(LogTemp, Display, TEXT("Player Character created"));
}

void AGWTPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize player systems
    InitializeInventory();
    InitializeGrimoire();
    InitializeProgression();
    
    UE_LOG(LogTemp, Display, TEXT("Player Character BeginPlay"));
}

void AGWTPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Player-specific tick logic
    
    // Update animation based on movement
    if (GetCharacterMovement())
    {
        float Speed = GetCharacterMovement()->Velocity.Size();
        // We would set animation properties here in a full implementation
    }
}

void AGWTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Set up gameplay key bindings
    check(PlayerInputComponent);
    
    // Movement bindings
    PlayerInputComponent->BindAxis("MoveForward", this, &AGWTPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AGWTPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AGWTPlayerCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AGWTPlayerCharacter::LookUp);
    
    // Spell casting bindings
    PlayerInputComponent->BindAction("Cast", IE_Pressed, this, &AGWTPlayerCharacter::StartSpellCast);
    PlayerInputComponent->BindAction("Cast", IE_Released, this, &AGWTPlayerCharacter::EndSpellCast);
    PlayerInputComponent->BindAction("NextSpell", IE_Pressed, this, &AGWTPlayerCharacter::CycleActiveSpellForward);
    PlayerInputComponent->BindAction("PrevSpell", IE_Pressed, this, &AGWTPlayerCharacter::CycleActiveSpellBackward);
    
    // UI bindings
    PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AGWTPlayerCharacter::ToggleInventory);
    PlayerInputComponent->BindAction("SpellEditor", IE_Pressed, this, &AGWTPlayerCharacter::ToggleSpellEditor);
    
    // Interaction binding
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AGWTPlayerCharacter::Interact);
    
    UE_LOG(LogTemp, Verbose, TEXT("Player input components set up"));
}

void AGWTPlayerCharacter::OnDeath()
{
    // Player-specific death behavior
    UE_LOG(LogTemp, Display, TEXT("Player character has died"));
    
    // Call base implementation
    Super::OnDeath();
    
    // Additional player death handling (game over, respawn, etc.)
    // In a full game, we would trigger a game over screen or respawn sequence
}

void AGWTPlayerCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // Find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        
        // Get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
        
        bMoveForward = true;
    }
    else
    {
        bMoveForward = false;
    }
}

void AGWTPlayerCharacter::MoveRight(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // Find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        
        // Get right vector 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
        
        bMoveRight = true;
    }
    else
    {
        bMoveRight = false;
    }
}

void AGWTPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AGWTPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AGWTPlayerCharacter::StartSpellCast()
{
    // Begin casting if not already casting
    if (!bIsCasting)
    {
        bIsCasting = true;
        
        // Visual feedback
        OnBeginCasting();
        
        UE_LOG(LogTemp, Verbose, TEXT("Player began casting spell"));
    }
}

void AGWTPlayerCharacter::EndSpellCast()
{
    // End casting and execute the spell
    if (bIsCasting)
    {
        bIsCasting = false;
        
        // Cast the spell
        CastSpell();
        
        // Visual feedback
        OnEndCasting();
        
        UE_LOG(LogTemp, Verbose, TEXT("Player finished casting spell"));
    }
}

void AGWTPlayerCharacter::CycleActiveSpell(bool bForward)
{
    // Make sure we have a grimoire
    if (!Grimoire)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot cycle spells: No grimoire"));
        return;
    }
    
    // Get number of spells
    int32 SpellCount = Grimoire->GetSpellCount();
    
    // If we have no spells, do nothing
    if (SpellCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot cycle spells: No spells in grimoire"));
        return;
    }
    
    // Cycle to next/previous spell
    if (bForward)
    {
        ActiveSpellIndex = (ActiveSpellIndex + 1) % SpellCount;
    }
    else
    {
        ActiveSpellIndex = (ActiveSpellIndex - 1 + SpellCount) % SpellCount;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Cycled to spell %d of %d"), ActiveSpellIndex + 1, SpellCount);
}

// Helper functions for the CycleActiveSpell with the input binding
void AGWTPlayerCharacter::CycleActiveSpellForward()
{
    CycleActiveSpell(true);
}

void AGWTPlayerCharacter::CycleActiveSpellBackward()
{
    CycleActiveSpell(false);
}

void AGWTPlayerCharacter::CastSpell()
{
    // Make sure we have a grimoire
    if (!Grimoire)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot cast spell: No grimoire"));
        return;
    }
    
    // Get the active spell
    UGWTSpell* ActiveSpell = Grimoire->GetSpell(ActiveSpellIndex);
    if (!ActiveSpell)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot cast spell: Invalid spell index"));
        return;
    }
    
    // Get the target using camera raycast
    FHitResult HitResult;
    bool bHit = GetCameraTarget(HitResult, CastingRange);
    
    // Cast the spell
    if (bHit && HitResult.GetActor())
    {
        // Cast spell with target
        ActiveSpell->Cast(this, HitResult.GetActor());
        UE_LOG(LogTemp, Display, TEXT("Cast spell at target: %s"), *HitResult.GetActor()->GetName());
    }
    else
    {
        // Cast spell without specific target
        ActiveSpell->Cast(this);
        UE_LOG(LogTemp, Display, TEXT("Cast spell with no target"));
    }
    
    // Visual feedback
    OnSpellCast(ActiveSpellIndex);
}

void AGWTPlayerCharacter::EquipItem(UGWTItem* Item)
{
    // Check if item is valid
    if (!Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Invalid item"));
        return;
    }
    
    // Check if item is equipment
    UGWTEquipment* Equipment = Cast<UGWTEquipment>(Item);
    if (!Equipment)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Not equipment"));
        return;
    }
    
    // Equip based on equipment slot
    switch (Equipment->EquipmentSlot)
    {
        case EGWTEquipmentSlot::Wand:
            EquipWand(Cast<UGWTWand>(Equipment));
            break;
            
        case EGWTEquipmentSlot::Hat:
            EquipHat(Cast<UGWTHat>(Equipment));
            break;
            
        case EGWTEquipmentSlot::Robe:
            EquipRobe(Cast<UGWTRobe>(Equipment));
            break;
            
        default:
            UE_LOG(LogTemp, Warning, TEXT("Equipment slot not implemented: %d"), 
                   (int32)Equipment->EquipmentSlot);
            break;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Equipped item: %s"), *Item->ItemName.ToString());
}

void AGWTPlayerCharacter::UnequipItem(EGWTEquipmentSlot Slot)
{
    // Handle unequipping based on slot
    switch (Slot)
    {
        case EGWTEquipmentSlot::Wand:
            // Clear wand mesh and effects
            WandMesh->SetStaticMesh(nullptr);
            break;
            
        case EGWTEquipmentSlot::Hat:
            // Clear hat mesh and effects
            HatMesh->SetStaticMesh(nullptr);
            break;
            
        case EGWTEquipmentSlot::Robe:
            // Clear robe mesh and effects
            break;
            
        default:
            UE_LOG(LogTemp, Warning, TEXT("Equipment slot not implemented: %d"), (int32)Slot);
            break;
    }
    
    UE_LOG(LogTemp, Display, TEXT("Unequipped item from slot: %d"), (int32)Slot);
}

void AGWTPlayerCharacter::ToggleInventory()
{
    // This would be implemented to show/hide the inventory UI
    UE_LOG(LogTemp, Display, TEXT("Toggle inventory"));
    
    // In a full implementation, we would get the player controller and toggle the inventory widget
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && PC->IsA(AGWTPlayerController::StaticClass()))
    {
        AGWTPlayerController* GWTPC = Cast<AGWTPlayerController>(PC);
        GWTPC->ToggleInventory();
    }
}

void AGWTPlayerCharacter::ToggleSpellEditor()
{
    // This would be implemented to show/hide the spell editor UI
    UE_LOG(LogTemp, Display, TEXT("Toggle spell editor"));
    
    // In a full implementation, we would get the player controller and toggle the spell editor widget
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && PC->IsA(AGWTPlayerController::StaticClass()))
    {
        AGWTPlayerController* GWTPC = Cast<AGWTPlayerController>(PC);
        GWTPC->ToggleSpellEditor();
    }
}

void AGWTPlayerCharacter::Interact()
{
    // Trace for interactive objects
    FHitResult HitResult;
    bool bHit = GetCameraTarget(HitResult, 200.0f); // Short interaction range
    
    if (bHit && HitResult.GetActor())
    {
        // Check if actor is interactive
        // In a full implementation, we would check for an interface
        UE_LOG(LogTemp, Display, TEXT("Interacting with: %s"), *HitResult.GetActor()->GetName());
        
        // Example interaction with a specific actor type
        if (HitResult.GetActor()->IsA(AGWTRoom::StaticClass()))
        {
            AGWTRoom* Room = Cast<AGWTRoom>(HitResult.GetActor());
            // Handle room interaction
        }
    }
    else
    {
        UE_LOG(LogTemp, Verbose, TEXT("No interactive object found"));
    }
}

bool AGWTPlayerCharacter::GetCameraTarget(FHitResult& OutHitResult, float MaxDistance)
{
    // Get the camera location and forward vector
    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector CameraForward = FollowCamera->GetForwardVector();
    
    // Calculate end point
    FVector TraceEnd = CameraLocation + (CameraForward * MaxDistance);
    
    // Set up collision query params
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // Don't hit self
    
    // Perform line trace
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        OutHitResult,
        CameraLocation,
        TraceEnd,
        ECC_Visibility, // Use visibility channel
        QueryParams
    );
    
    return bHit;
}

void AGWTPlayerCharacter::ApplyStatusEffect(FGWTStatusEffect Effect)
{
    // Call base implementation
    Super::ApplyStatusEffect(Effect);
    
    // Player-specific status effect handling
    // For example, providing feedback to the player UI
    
    // In a full implementation, we would update the UI to show status effects
}

void AGWTPlayerCharacter::InitializeInventory()
{
    // Create inventory if it doesn't exist
    if (!Inventory)
    {
        Inventory = NewObject<UGWTInventory>(this);
        
        // Add some starting gold
        Inventory->AddGold(50);
        
        UE_LOG(LogTemp, Display, TEXT("Player inventory initialized with 50 gold"));
    }
}

void AGWTPlayerCharacter::InitializeGrimoire()
{
    // Create grimoire if it doesn't exist
    if (!Grimoire)
    {
        Grimoire = NewObject<UGWTGrimoire>(this);
        
        // Create a starter spell
        UGWTSpell* StarterSpell = Grimoire->CreateNewSpell(TEXT("Firebolt"));
        
        UE_LOG(LogTemp, Display, TEXT("Player grimoire initialized with starter spell"));
    }
}

void AGWTPlayerCharacter::InitializeProgression()
{
    // Create progression system if it doesn't exist
    if (!Progression)
    {
        Progression = NewObject<UGWTPlayerProgression>(this);
        
        UE_LOG(LogTemp, Display, TEXT("Player progression initialized"));
    }
}