#include "SFMinimapCaptureActor.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

ASFMinimapCaptureActor::ASFMinimapCaptureActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	SetRootComponent(CaptureComponent);
	CaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	CaptureComponent->OrthoWidth = OrthoWidthMeters * 100.0f;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	CaptureComponent->bCaptureEveryFrame = true;
	CaptureComponent->bCaptureOnMovement = true;
}

void ASFMinimapCaptureActor::BeginPlay()
{
	Super::BeginPlay();

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(512, 512);
	RenderTarget->ClearColor = FLinearColor(0.05f, 0.07f, 0.09f);
	RenderTarget->UpdateResourceImmediate(true);
	CaptureComponent->TextureTarget = RenderTarget;
	SetOrthoWidthMeters(OrthoWidthMeters);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		SetFollowTarget(PC->GetPawn());
	}
}

void ASFMinimapCaptureActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AActor* Target = FollowTarget.Get();
	if (!Target)
	{
		return;
	}

	const FVector TargetLocation = Target->GetActorLocation();
	SetActorLocation(FVector(TargetLocation.X, TargetLocation.Y, TargetLocation.Z + CaptureHeightCm));

	if (bNorthUp)
	{
		SetActorRotation(FRotator(-90.0f, -90.0f, 0.0f));
	}
	else
	{
		const float Yaw = Target->GetActorRotation().Yaw;
		SetActorRotation(FRotator(-90.0f, Yaw - 90.0f, 0.0f));
	}
}

void ASFMinimapCaptureActor::SetFollowTarget(AActor* Target)
{
	FollowTarget = Target;
}

void ASFMinimapCaptureActor::SetNorthUp(bool bEnabled)
{
	bNorthUp = bEnabled;
}

void ASFMinimapCaptureActor::SetOrthoWidthMeters(float WidthMeters)
{
	OrthoWidthMeters = FMath::Max(50.0f, WidthMeters);
	if (CaptureComponent)
	{
		CaptureComponent->OrthoWidth = OrthoWidthMeters * 100.0f;
	}
}
