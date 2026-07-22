#include "SFDestinationMarker.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ASFDestinationMarker::ASFDestinationMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	FinishSphere = CreateDefaultSubobject<USphereComponent>(TEXT("FinishSphere"));
	SetRootComponent(FinishSphere);
	FinishSphere->SetSphereRadius(DestinationRadiusMeters * 100.0f);
	FinishSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FinishSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	FinishSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	MarkerMesh->SetupAttachment(FinishSphere);
	MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		MarkerMesh->SetStaticMesh(SphereMesh.Object);
		MarkerMesh->SetRelativeScale3D(FVector(2.0f));
	}
}

void ASFDestinationMarker::SetDestinationRadiusMeters(float RadiusMeters)
{
	DestinationRadiusMeters = FMath::Max(1.0f, RadiusMeters);
	if (FinishSphere)
	{
		FinishSphere->SetSphereRadius(DestinationRadiusMeters * 100.0f);
	}
}
