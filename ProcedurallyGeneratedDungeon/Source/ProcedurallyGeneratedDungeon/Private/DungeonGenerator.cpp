#include "DungeonGenerator.h"
#include "MasterRoom.h"
#include "WallDoorActor.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
    PrimaryActorTick.bCanEverTick = true; // Enable ticking only if necessary
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
    Super::BeginPlay();
    InitializeDungeonGeneration();
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Initializes the dungeon generation process
void ADungeonGenerator::InitializeDungeonGeneration()
{
    SetSeed();
    if (!SetStartRoom())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set the start room. Dungeon generation aborted."));
        return;
    }
    SpawnNextRoom();
}

// Sets the random seed for the dungeon
void ADungeonGenerator::SetSeed()
{
    if (Seed == -1)
    {
        Seed = FMath::RandRange(0, 100000);
    }
    FMath::RandInit(Seed);
    UE_LOG(LogTemp, Warning, TEXT("Seed: %d"), Seed);
}

// Sets the initial room of the dungeon
bool ADungeonGenerator::SetStartRoom()
{
    if (StartRoomsList.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("BaseRoomsList is empty! Cannot set start room."));
        return false;
    }

    PopulateRoomsList(StartRoomsList);

    if (RoomsList.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("RoomsList is still empty after population!"));
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Spawn a random room from RoomsList
    AActor* SpawnedRoom = GetWorld()->SpawnActor<AActor>(
        RoomsList[FMath::RandRange(0, RoomsList.Num() - 1)],
        GetActorTransform(),
        SpawnParams
    );

    LatestRoom = Cast<AMasterRoom>(SpawnedRoom);
    ProcessRoomWalls(LatestRoom);
    return LatestRoom != nullptr;
}

// Populates the RoomsList based on BaseRoomsList
void ADungeonGenerator::PopulateRoomsList(	TArray<TSubclassOf<AMasterRoom>> ListOfRooms)
{
    
    if (!RoomsList.IsEmpty())
    {
        RoomsList.Empty();
    }
    if (ListOfRooms.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ListOfRooms is empty! Cannot populate RoomsList."));
        return;
    }
    for (TSubclassOf Room : ListOfRooms)
    {
        if (Room->IsChildOf<AMasterRoom>())
        {
            RoomsList.Add(Room);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Class %s is not a valid subclass of AMasterRoom"), *Room->GetName());
        }
    }
}

// Spawns the next room in the dungeon
bool ADungeonGenerator::SpawnNextRoom()
{
    if (ExitsList.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No exits available to spawn a new room."));
        return false;
    }

    PopulateRoomsList(BaseRoomsList);
    AActor* SpawnedRoom = TrySpawnRoomAtExit();

    if (!SpawnedRoom)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn a new room at the selected exit."));
        return false;
    }

    --RoomAmount;

    if ((RoomAmount-2) > 0)
    {
        return SpawnNextRoom();
    }
    else
    {
        SpawnBossRoom();
        FinalizeDungeonGeneration();
        return true;
    }
}

void ADungeonGenerator::SpawnBossRoom()
{
    if (BossRoomsList.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("BossRoomsList is empty! Cannot spawn boss room."));
        return;
    }
    PopulateRoomsList(BossRoomsList);
    TrySpawnRoomAtExit();
}

// Attempts to spawn a room at any available exit
AActor* ADungeonGenerator::TrySpawnRoomAtExit()
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (RoomsList.IsEmpty())
    {
        PopulateRoomsList(BaseRoomsList);
    }
    
    const int NumRooms = RoomsList.Num();
    const int NumExits = ExitsList.Num();
    const int MaxRetries = NumRooms > 1 ? NumRooms * NumExits : FMath::Max(NumExits * 3, 10);
    int RetryCount = 0;
    // Iterate over exits to find a valid one
    while (RetryCount < MaxRetries && !ExitsList.IsEmpty())
    {
        // Randomly select an exit from the available exits
        UArrowComponent* SelectedExit = Cast<UArrowComponent>(ExitsList[FMath::RandRange(0, ExitsList.Num() - 1)]);

        AActor* SpawnedRoom = GetWorld()->SpawnActor<AActor>(
            RoomsList[FMath::RandRange(0, RoomsList.Num() - 1)],
            SelectedExit->GetComponentTransform(),
            SpawnParams
        );

        LatestRoom = Cast<AMasterRoom>(SpawnedRoom);

        if (LatestRoom && !IsOverlapping())
        {
            ConvertWallToDoor(SelectedExit); 
            ExitsList.Remove(SelectedExit);
            // add new exits to the list
            ProcessRoomWalls(LatestRoom);
            return SpawnedRoom;
        }

        if (LatestRoom)
        {
            LatestRoom->Destroy();
        }

        ++RetryCount;
    }

    UE_LOG(LogTemp, Warning, TEXT("Max retries reached or no valid exits left. Failed to find a valid location."));
    return nullptr;
}

// Converts a wall to a door at a specified exit point
void ADungeonGenerator::ConvertWallToDoor(UArrowComponent* ExitPoint)
{
    if (!ExitPoint) return;

    AWallDoorActor* WallDoor = Cast<AWallDoorActor>(ExitPoint->GetOwner());
    if (WallDoor)
    {
        WallDoor->ConvertToDoor();
        UE_LOG(LogTemp, Warning, TEXT("Wall converted to door for room at %s"), *WallDoor->GetName());
    }
}

// Processes walls in the room and adds them to ExitsList if valid
void ADungeonGenerator::ProcessRoomWalls(AMasterRoom* Room)
{
    if (!Room || !Room->WallOrDoorFolder) return;

    TArray<USceneComponent*> ChildActors;
    Room->WallOrDoorFolder->GetChildrenComponents(false, ChildActors);

    for (USceneComponent* Child : ChildActors)
    {
        if (AWallDoorActor* WallDoor = Cast<AWallDoorActor>(Cast<UChildActorComponent>(Child)->GetChildActor()))
        {
            if (WallDoor->ArrowComp && !WallDoor->IsDoor)
            {
                ExitsList.Add(WallDoor->ArrowComp);
            }
        }
    }
}

// Finalizes dungeon generation
void ADungeonGenerator::FinalizeDungeonGeneration()
{
    CloseHoles();
    UE_LOG(LogTemp, Warning, TEXT("Dungeon generation finished."));
}

void ADungeonGenerator::CloseHoles()
{
    for (USceneComponent* Hole : ExitsList)
    {
        // Check if the exit overlaps with any other room area
        TArray<AActor*> OverlappingActors;
        Cast<AWallDoorActor>(Hole->GetOwner())->BoxComp->GetOverlappingActors(OverlappingActors);

        bool bOverlapsWithRoom = false;
        for (AActor* Actor : OverlappingActors)
        {
            if (Actor->IsA(AMasterRoom::StaticClass()))
            {
                bOverlapsWithRoom = true;
                break;
            }
        }

        if (bOverlapsWithRoom)
        {
            // Randomly decide to turn it into a door
            if (FMath::RandBool())
            {
                ConvertWallToDoor(Cast<UArrowComponent>(Hole));
                continue; // Skip spawning a wall
            }
        }

        // Spawn a wall if it's not turned into a door
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    }
}


bool ADungeonGenerator::IsOverlapping()
{
    if (!LatestRoom) return true;

    TArray<AActor*> OverlappingActors;
    LatestRoom->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor->IsA(AMasterRoom::StaticClass()))
        {
            return true;
        }
    }
    return false;
}
