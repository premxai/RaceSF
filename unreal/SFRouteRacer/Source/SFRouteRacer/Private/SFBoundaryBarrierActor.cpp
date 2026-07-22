#include "SFBoundaryBarrierActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ASFBoundaryBarrierActor::ASFBoundaryBarrierActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BlockingVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingVolume"));
	SetRootComponent(BlockingVolume);
	BlockingVolume->SetBoxExtent(FVector(200.0f, 50.0f, 150.0f));
	BlockingVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockingVolume->SetCollisionResponseToAllChannels(ECR_Block);

	BarrierMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrierMesh"));
	BarrierMesh->SetupAttachment(BlockingVolume);
	BarrierMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BarrierMesh->SetStaticMesh(CubeMesh.Object);
		BarrierMesh->SetRelativeScale3D(FVector(4.0f, 1.0f, 3.0f));
	}
}

void ASFBoundaryBarrierActor::Configure(
	const FVector& Location, const FRotator& Rotation, float LengthCm, float HeightCm)
{
	SetActorLocationAndRotation(Location, Rotation);
	const float HalfLength = FMath::Max(50.0f, LengthCm * 0.5f);
	const float HalfHeight = FMath::Max(50.0f, HeightCm * 0.5f);
	BlockingVolume->SetBoxExtent(FVector(HalfLength, 40.0f, HalfHeight));
	if (BarrierMesh)
	{
		BarrierMesh->SetRelativeScale3D(FVector(LengthCm / 100.0f, 0.8f, HeightCm / 100.0f));
	}
}
