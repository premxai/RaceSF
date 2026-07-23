#include "SFDestinationMarker.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
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

	PoleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PoleMesh"));
	PoleMesh->SetupAttachment(FinishSphere);
	PoleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PoleMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	PoleMesh->SetRelativeScale3D(FVector(0.35f, 0.35f, 8.0f));

	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	MarkerMesh->SetupAttachment(FinishSphere);
	MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MarkerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 900.0f));
	MarkerMesh->SetRelativeScale3D(FVector(3.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (CylinderMesh.Succeeded())
	{
		PoleMesh->SetStaticMesh(CylinderMesh.Object);
	}
	if (SphereMesh.Succeeded())
	{
		MarkerMesh->SetStaticMesh(SphereMesh.Object);
	}
}

void ASFDestinationMarker::BeginPlay()
{
	Super::BeginPlay();
	ApplyVisuals();
}

void ASFDestinationMarker::SetDestinationRadiusMeters(float RadiusMeters)
{
	DestinationRadiusMeters = FMath::Max(1.0f, RadiusMeters);
	if (FinishSphere)
	{
		FinishSphere->SetSphereRadius(DestinationRadiusMeters * 100.0f);
	}
}

void ASFDestinationMarker::SetMarkerStyle(ESFLandmarkMarkerStyle Style)
{
	MarkerStyle = Style;
	ApplyVisuals();

	// Start markers are visual only — don't steal finish overlaps.
	if (FinishSphere)
	{
		const bool bFinish = (MarkerStyle == ESFLandmarkMarkerStyle::Finish);
		FinishSphere->SetCollisionEnabled(bFinish ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void ASFDestinationMarker::ApplyVisuals()
{
	const TCHAR* MaterialPath = (MarkerStyle == ESFLandmarkMarkerStyle::Finish)
		? TEXT("/Game/Materials/M_SFCarUnlit.M_SFCarUnlit")
		: TEXT("/Game/Materials/M_SFStartUnlit.M_SFStartUnlit");

	UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, MaterialPath);
	if (!Material)
	{
		Material = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Game/Materials/M_SFGrayboxUnlit.M_SFGrayboxUnlit"));
	}
	if (!Material)
	{
		Material = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}

	if (Material)
	{
		if (PoleMesh)
		{
			PoleMesh->SetMaterial(0, Material);
		}
		if (MarkerMesh)
		{
			MarkerMesh->SetMaterial(0, Material);
		}
	}

	if (MarkerStyle == ESFLandmarkMarkerStyle::Start)
	{
		if (PoleMesh)
		{
			PoleMesh->SetRelativeScale3D(FVector(0.28f, 0.28f, 5.5f));
			PoleMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 280.0f));
		}
		if (MarkerMesh)
		{
			MarkerMesh->SetRelativeScale3D(FVector(2.4f));
			MarkerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 620.0f));
		}
	}
	else
	{
		if (PoleMesh)
		{
			PoleMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 9.0f));
			PoleMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 450.0f));
		}
		if (MarkerMesh)
		{
			MarkerMesh->SetRelativeScale3D(FVector(4.0f));
			MarkerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 1000.0f));
		}
	}
}
