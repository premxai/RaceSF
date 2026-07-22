#include "SFRoadNetworkActor.h"

#include "Engine/GameInstance.h"
#include "ProceduralMeshComponent.h"
#include "SFMapDataSubsystem.h"
#include "SFProceduralRoadGeometryBuilder.h"
#include "SFRouteRacer.h"

ASFRoadNetworkActor::ASFRoadNetworkActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RoadMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RoadMesh"));
	SetRootComponent(RoadMesh);
	RoadMesh->bUseAsyncCooking = true;
	RoadMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RoadMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void ASFRoadNetworkActor::BeginPlay()
{
	Super::BeginPlay();
	if (bBuildOnBeginPlay)
	{
		RebuildFromMapData();
	}
}

void ASFRoadNetworkActor::RebuildFromMapData()
{
	UGameInstance* GameInstance = GetGameInstance();
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData)
	{
		UE_LOG(LogSFRace, Error, TEXT("Road network cannot build: map subsystem missing"));
		return;
	}

	if (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport())
	{
		UE_LOG(LogSFRace, Error, TEXT("Road network cannot build: %s"), *MapData->GetLastError());
		return;
	}

	if (!GeometryBuilder)
	{
		GeometryBuilder = NewObject<USFProceduralRoadGeometryBuilder>(this);
	}

	RoadMesh->ClearAllMeshSections();
	RoadMesh->bUseAsyncCooking = false;

	int32 SectionIndex = 0;
	int32 Built = 0;
	for (const FSFRoadEdgeData& Edge : MapData->GetEdges())
	{
		if (!Edge.bDriveable || Edge.bTunnel)
		{
			continue;
		}
		if (MaxEdgesToBuild > 0 && Built >= MaxEdgesToBuild)
		{
			break;
		}
		if (GeometryBuilder->BuildRoadMesh(this, Edge, RoadMesh, SectionIndex))
		{
			++SectionIndex;
			++Built;
		}
	}

	RoadMesh->MarkRenderStateDirty();
	UE_LOG(LogSFRace, Log, TEXT("Road network built %d edge meshes (%d sections)"), Built, SectionIndex);
}
