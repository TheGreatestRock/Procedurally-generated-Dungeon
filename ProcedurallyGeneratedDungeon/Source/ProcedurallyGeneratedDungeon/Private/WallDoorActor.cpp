#include "WallDoorActor.h"
#include "Components/ArrowComponent.h"

// Sets default values
AWallDoorActor::AWallDoorActor()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);

    WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
    WallMesh->SetupAttachment(RootComponent);

    ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Exit"));
    ArrowComp->SetupAttachment(DoorMesh);

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    BoxComp->SetupAttachment(ArrowComp);

    IsDoor = false; // Start as a wall
}

// Called when the game starts or when spawned
void AWallDoorActor::BeginPlay()
{
    Super::BeginPlay();
}

// Converts the wall to a door
void AWallDoorActor::ConvertToDoor()
{
    if (!IsDoor)
    {
        IsDoor = true;
        WallMesh->DestroyComponent();
        UE_LOG(LogTemp, Warning, TEXT("Wall at %s converted to a door."), *GetName());
    }
}
