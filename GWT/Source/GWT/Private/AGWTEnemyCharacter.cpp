// AGWTEnemyCharacter.cpp
// Implementation of the base enemy character

#include "UGWTEnemyCharacter.h"
#include "UGWTSpell.h"
#include "AGWTRoom.h"
#include "AGWTPlayerCharacter.h"
#include "AGWTPlayerController.h"
#include "AGWTGameMode.h"
#include "AGWTGameState.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Components/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"

AGWTEnemyCharacter::AGWTEnemyCharacter()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Set default enemy properties
    EnemyType = EGWTEnemyType::Goblin;
    ExperienceValue = 10;
    GoldValue = 5;
    DetectionRadius = 1000.0f;
    AttackRange = 200.0f;
    MaxAggroRange = 2000.0f;
    MinAttackCooldown = 2.0f;
    MaxAttackCooldown = 4.0f;

    // Set default stats - lower than player but more numerous
    MaxHealth = 75.0f;
    CurrentHealth = MaxHealth;
    MaxMana = 50.0f;
    CurrentMana = MaxMana;
    ManaRegenRate = 3.0f;
    MovementSpeed = 500.0f;

    // Create AI perception component
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

    // Set up sight config
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    SightConfig->SightRadius = DetectionRadius;
    SightConfig->LoseSightRadius = DetectionRadius * 1.5f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    if (PerceptionComponent)
    {
        PerceptionComponent->ConfigureSense(*SightConfig);
        PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }

    // Create pawn sensing component for simpler sensing
    SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComponent"));
    SensingComponent->SetPeripheralVisionAngle(90.0f);
    SensingComponent->SightRadius = DetectionRadius;
    SensingComponent->HearingThreshold = 500.0f;
    SensingComponent->LOSHearingThreshold = 1000.0f;

    // Set movement speed
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->MaxWalkSpeed = MovementSpeed;
    }

    // Initialize AI state
    bIsAggressive = true;
    bIsPatrolling = true;
    bIsAttacking = false;
    CurrentTarget = nullptr;

    // Log initialization
    UE_LOG(LogTemp, Display, TEXT("Enemy Character created of type %s"),
        *UEnum::GetValueAsString(EnemyType));
}

void AGWTEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize AI behavior
    InitializeAI();

    // Set up patrol points
    SetupPatrolPoints();

    // Create spells
    InitializeSpells();

    // Register sensory events
    if (SensingComponent)
    {
        SensingComponent->OnSeePawn.AddDynamic(this, &AGWTEnemyCharacter::OnSeePlayer);
        SensingComponent->OnHearNoise.AddDynamic(this, &AGWTEnemyCharacter::OnHearNoise);
    }

    // Set up initial patrol timer
    GetWorld()->GetTimerManager().SetTimer(
        PatrolTimerHandle,
        this,
        &AGWTEnemyCharacter::PatrolTimerCallback,
        1.0f, // Start patrolling after 1 second
        false // Don't loop (will be reset in callback)
    );

    UE_LOG(LogTemp, Verbose, TEXT("Enemy Character BeginPlay: %s"), *GetName());
}

void AGWTEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // AI behavior logic
    if (CurrentTarget)
    {
        // Check if target is still in range
        if (!IsTargetInRange(MaxAggroRange))
        {
            // Target is out of aggro range, lose interest
            UE_LOG(LogTemp, Verbose, TEXT("Enemy %s lost target - out of range"), *GetName());
            CurrentTarget = nullptr;
            bIsAttacking = false;

            // Start patrolling again
            if (!GetWorld()->GetTimerManager().IsTimerActive(PatrolTimerHandle))
            {
                GetWorld()->GetTimerManager().SetTimer(
                    PatrolTimerHandle,
                    this,
                    &AGWTEnemyCharacter::PatrolTimerCallback,
                    1.0f,
                    false
                );
            }
        }
        else
        {
            // Target is in range, decide whether to chase or attack
            if (IsTargetInRange(AttackRange))
            {
                // Target is in attack range
                if (!bIsAttacking)
                {
                    // Start attacking
                    bIsAttacking = true;
                    AttackTarget();
                }
            }
            else
            {
                // Target is in sight but not in attack range, chase it
                bIsAttacking = false;
                ChaseTarget();
            }
        }
    }
    else if (bIsPatrolling)
    {
        // No target, continue patrolling
        DetectPlayer();
    }
}

void AGWTEnemyCharacter::OnDeath()
{
    // Enemy-specific death handling
    UE_LOG(LogTemp, Display, TEXT("Enemy %s has died"), *GetName());

    // Stop all timers
    GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);

    // Grant rewards to player(s)
    DropLoot();
    GrantExperience();

    // Update game state
    AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
    if (GameState && GameState->IsA(AGWTGameState::StaticClass()))
    {
        AGWTGameState* GWTGameState = Cast<AGWTGameState>(GameState);
        if (GWTGameState)
        {
            GWTGameState->EnemyKilled();
        }
    }

    // Update room if in one
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, AGWTRoom::StaticClass());

    for (AActor* Actor : OverlappingActors)
    {
        AGWTRoom* Room = Cast<AGWTRoom>(Actor);
        if (Room)
        {
            // Decrement enemy count in the room
            // If this was the last enemy, mark room as cleared
            Room->OnEnemyKilled(this);
            break;
        }
    }

    // Call base implementation
    Super::OnDeath();
}

void AGWTEnemyCharacter::DetectPlayer()
{
    // This is primarily handled by the pawn sensing component
    // But we can manually check for players in detection radius

    APawn* NearestPlayer = GetClosestPlayer();
    if (NearestPlayer)
    {
        float Distance = FVector::Dist(GetActorLocation(), NearestPlayer->GetActorLocation());

        if (Distance <= DetectionRadius)
        {
            // Check line of sight
            FHitResult HitResult;
            bool bHit = GetWorld()->LineTraceSingleByChannel(
                HitResult,
                GetActorLocation(),
                NearestPlayer->GetActorLocation(),
                ECC_Visibility
            );

            if (!bHit || HitResult.GetActor() == NearestPlayer)
            {
                // Player is visible, set as target
                CurrentTarget = NearestPlayer;
                UE_LOG(LogTemp, Verbose, TEXT("Enemy %s detected player %s"),
                    *GetName(), *NearestPlayer->GetName());
            }
        }
    }
}

void AGWTEnemyCharacter::ChaseTarget()
{
    // Move toward the target
    if (CurrentTarget)
    {
        // Get movement component
        UCharacterMovementComponent* MoveComp = GetCharacterMovement();
        if (MoveComp)
        {
            // Calculate direction to target
            FVector Direction = CurrentTarget->GetActorLocation() - GetActorLocation();
            Direction.Normalize();

            // Move in that direction
            AddMovementInput(Direction);

            // Set view rotation to face target
            FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(
                GetActorLocation(),
                CurrentTarget->GetActorLocation()
            );
            NewRotation.Pitch = 0.0f; // Keep level
            NewRotation.Roll = 0.0f;  // No roll

            SetActorRotation(NewRotation);

            UE_LOG(LogTemp, Verbose, TEXT("Enemy %s chasing target %s"),
                *GetName(), *CurrentTarget->GetName());
        }
    }
}

void AGWTEnemyCharacter::AttackTarget()
{
    // Start attack sequence if not already attacking
    if (!GetWorld()->GetTimerManager().IsTimerActive(AttackTimerHandle))
    {
        // Set attack cooldown
        CurrentAttackCooldown = GetRandomAttackCooldown();

        // Cast a spell at the target
        CastSpell();

        // Set timer for next attack
        GetWorld()->GetTimerManager().SetTimer(
            AttackTimerHandle,
            this,
            &AGWTEnemyCharacter::AttackTimerCallback,
            CurrentAttackCooldown,
            false
        );

        UE_LOG(LogTemp, Display, TEXT("Enemy %s attacking target %s with cooldown %.1f"),
            *GetName(), *CurrentTarget->GetName(), CurrentAttackCooldown);
    }
}

void AGWTEnemyCharacter::Patrol()
{
    // Get the next patrol point
    FVector TargetLocation = GetNextPatrolPoint();

    // Move toward that point
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        // Calculate direction to patrol point
        FVector Direction = TargetLocation - GetActorLocation();
        Direction.Normalize();

        // Move in that direction
        AddMovementInput(Direction);

        // Set view rotation to face direction
        FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(
            GetActorLocation(),
            TargetLocation
        );
        NewRotation.Pitch = 0.0f; // Keep level
        NewRotation.Roll = 0.0f;  // No roll

        SetActorRotation(NewRotation);

        UE_LOG(LogTemp, Verbose, TEXT("Enemy %s patrolling to point %d"),
            *GetName(), CurrentPatrolIndex);
    }
}

void AGWTEnemyCharacter::SelectSpell()
{
    // Select a random spell to cast
    if (EnemySpells.Num() > 0)
    {
        CurrentSpellIndex = FMath::RandRange(0, EnemySpells.Num() - 1);
        UE_LOG(LogTemp, Verbose, TEXT("Enemy %s selected spell %d of %d"),
            *GetName(), CurrentSpellIndex + 1, EnemySpells.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s has no spells to cast"), *GetName());
    }
}

void AGWTEnemyCharacter::CastSpell()
{
    // Select a spell if needed
    if (EnemySpells.Num() <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s has no spells to cast"), *GetName());
        return;
    }

    // Make sure we have a valid spell index
    if (CurrentSpellIndex < 0 || CurrentSpellIndex >= EnemySpells.Num())
    {
        SelectSpell();
    }

    // Get the spell
    UGWTSpell* Spell = EnemySpells[CurrentSpellIndex];
    if (Spell)
    {
        // Cast the spell at the current target
        Spell->Cast(this, CurrentTarget);
        UE_LOG(LogTemp, Display, TEXT("Enemy %s cast spell %s at %s"),
            *GetName(), *Spell->SpellName.ToString(), *CurrentTarget->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s has invalid spell at index %d"),
            *GetName(), CurrentSpellIndex);
    }
}

void AGWTEnemyCharacter::DropLoot()
{
    // In a full implementation, this would spawn item pickups
    // For this example, we'll just log that loot would be dropped

    UE_LOG(LogTemp, Display, TEXT("Enemy %s would drop loot worth %d gold"),
        *GetName(), GoldValue);

    // Determine players who should receive loot
    TArray<AActor*> NearbyPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGWTPlayerCharacter::StaticClass(), NearbyPlayers);

    for (AActor* Player : NearbyPlayers)
    {
        // Check if player is in loot range
        float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
        if (Distance <= 1000.0f) // Loot range
        {
            // Grant gold to player
            AGWTPlayerCharacter* PlayerChar = Cast<AGWTPlayerCharacter>(Player);
            if (PlayerChar && PlayerChar->Inventory)
            {
                PlayerChar->Inventory->AddGold(GoldValue);
                UE_LOG(LogTemp, Display, TEXT("Granted %d gold to player %s"),
                    GoldValue, *PlayerChar->GetName());
            }
        }
    }
}

void AGWTEnemyCharacter::GrantExperience()
{
    // In a full implementation, this would award XP to the player
    // For this example, we'll just log that XP would be granted

    UE_LOG(LogTemp, Display, TEXT("Enemy %s would grant %d experience"),
        *GetName(), ExperienceValue);

    // Determine players who should receive XP
    TArray<AActor*> NearbyPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGWTPlayerCharacter::StaticClass(), NearbyPlayers);

    for (AActor* Player : NearbyPlayers)
    {
        // Check if player is in XP range
        float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
        if (Distance <= 1500.0f) // XP range
        {
            // Grant XP to player
            AGWTPlayerCharacter* PlayerChar = Cast<AGWTPlayerCharacter>(Player);
            if (PlayerChar && PlayerChar->Progression)
            {
                PlayerChar->Progression->AddXP(ExperienceValue);
                UE_LOG(LogTemp, Display, TEXT("Granted %d XP to player %s"),
                    ExperienceValue, *PlayerChar->GetName());
            }
        }
    }
}

void AGWTEnemyCharacter::OnSeePlayer(APawn* Pawn)
{
    // Check if the pawn is a player
    if (Pawn && Pawn->IsA(AGWTPlayerCharacter::StaticClass()))
    {
        // Set as current target
        CurrentTarget = Pawn;
        UE_LOG(LogTemp, Display, TEXT("Enemy %s saw player %s"),
            *GetName(), *Pawn->GetName());
    }
}

void AGWTEnemyCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
    // Check if the pawn is a player
    if (PawnInstigator && PawnInstigator->IsA(AGWTPlayerCharacter::StaticClass()))
    {
        // Set as current target if close enough
        float Distance = FVector::Dist(GetActorLocation(), Location);
        if (Distance <= DetectionRadius)
        {
            CurrentTarget = PawnInstigator;
            UE_LOG(LogTemp, Display, TEXT("Enemy %s heard player %s"),
                *GetName(), *PawnInstigator->GetName());
        }
    }
}

void AGWTEnemyCharacter::SetDifficultyLevel(int32 WaveNumber)
{
    // Scale enemy stats based on wave number
    float DifficultyMultiplier = 1.0f + (WaveNumber * 0.2f); // +20% per wave

    // Scale health and damage
    MaxHealth = 75.0f * DifficultyMultiplier;
    CurrentHealth = MaxHealth;

    // Scale rewards
    ExperienceValue = FMath::RoundToInt(10.0f * DifficultyMultiplier);
    GoldValue = FMath::RoundToInt(5.0f * DifficultyMultiplier);

    UE_LOG(LogTemp, Display, TEXT("Enemy %s difficulty set for wave %d: Health %.1f, XP %d, Gold %d"),
        *GetName(), WaveNumber, MaxHealth, ExperienceValue, GoldValue);
}

bool AGWTEnemyCharacter::IsTargetInRange(float Range) const
{
    // Check if target exists and is in range
    if (CurrentTarget)
    {
        float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
        return Distance <= Range;
    }

    return false;
}

APawn* AGWTEnemyCharacter::GetClosestPlayer() const
{
    // Find all players
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGWTPlayerCharacter::StaticClass(), Players);

    // Find closest player
    APawn* ClosestPlayer = nullptr;
    float ClosestDistance = MAX_FLT;

    for (AActor* Player : Players)
    {
        float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestPlayer = Cast<APawn>(Player);
            ClosestDistance = Distance;
        }
    }

    return ClosestPlayer;
}

void AGWTEnemyCharacter::InitializeAI()
{
    // Set up AI behavior
    // In a full implementation, this would use a behavior tree or similar

    // For this example, we'll use simple timers and state machines
    CurrentAttackCooldown = GetRandomAttackCooldown();
    bIsAggressive = true;
    bIsPatrolling = true;
    bIsAttacking = false;
    CurrentTarget = nullptr;

    UE_LOG(LogTemp, Verbose, TEXT("Enemy %s AI initialized"), *GetName());
}

void AGWTEnemyCharacter::SetupPatrolPoints()
{
    // In a full implementation, this would create patrol points based on
    // the room the enemy is spawned in

    // For this example, we'll create some simple patrol points around spawn location
    PatrolPoints.Empty();

    // Get the spawn location
    FVector SpawnLocation = GetActorLocation();

    // Add patrol points in a circle around spawn
    const int32 NumPoints = 4;
    const float Radius = 300.0f;

    for (int32 i = 0; i < NumPoints; i++)
    {
        float Angle = (static_cast<float>(i) / NumPoints) * 2.0f * PI;
        FVector Point = SpawnLocation + FVector(
            Radius * FMath::Cos(Angle),
            Radius * FMath::Sin(Angle),
            0.0f
        );

        // Make sure point is valid (on navmesh)
        FNavLocation NavLocation;
        UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        if (NavSystem && NavSystem->ProjectPointToNavigation(Point, NavLocation))
        {
            PatrolPoints.Add(NavLocation.Location);
        }
        else
        {
            // If not on navmesh, just add the original point
            PatrolPoints.Add(Point);
        }
    }

    // Start with a random patrol point
    CurrentPatrolIndex = FMath::RandRange(0, PatrolPoints.Num() - 1);

    UE_LOG(LogTemp, Verbose, TEXT("Enemy %s patrol points set up: %d points"),
        *GetName(), PatrolPoints.Num());
}

float AGWTEnemyCharacter::GetRandomAttackCooldown() const
{
    // Return a random cooldown between min and max
    return FMath::FRandRange(MinAttackCooldown, MaxAttackCooldown);
}

void AGWTEnemyCharacter::AttackTimerCallback()
{
    // Time to attack again
    if (CurrentTarget && IsTargetInRange(AttackRange))
    {
        // Attack the target
        AttackTarget();
    }
    else
    {
        // Target out of range, reset attack state
        bIsAttacking = false;
    }
}

void AGWTEnemyCharacter::PatrolTimerCallback()
{
    // Time to move to the next patrol point
    if (!CurrentTarget && bIsPatrolling)
    {
        // Move to next patrol point
        Patrol();

        // Set timer for next patrol move
        GetWorld()->GetTimerManager().SetTimer(
            PatrolTimerHandle,
            this,
            &AGWTEnemyCharacter::PatrolTimerCallback,
            3.0f, // Move to next point every 3 seconds
            false
        );
    }
}

FVector AGWTEnemyCharacter::GetNextPatrolPoint() const
{
    // Get the current patrol point
    if (PatrolPoints.Num() > 0 && PatrolPoints.IsValidIndex(CurrentPatrolIndex))
    {
        return PatrolPoints[CurrentPatrolIndex];
    }

    // Fallback to current location
    return GetActorLocation();
}

void AGWTEnemyCharacter::InitializeSpells()
{
    // Create some basic spells for the enemy
    // In a full implementation, this would be driven by data

    // Create a basic attack spell
    UGWTSpell* AttackSpell = NewObject<UGWTSpell>(this);
    if (AttackSpell)
    {
        // Set up spell properties
        AttackSpell->SpellName = FText::FromString("Enemy Fireball");
        AttackSpell->SpellDescription = FText::FromString("A simple fireball spell");
        AttackSpell->BaseDamage = 10.0f;
        AttackSpell->TotalManaCost = 5.0f;

        // In a full implementation, we would add nodes to this spell
        // For this example, we'll just create a placeholder spell

        // Add to enemy spells
        EnemySpells.Add(AttackSpell);
    }

    UE_LOG(LogTemp, Display, TEXT("Enemy %s spells initialized: %d spells"),
        *GetName(), EnemySpells.Num());
}