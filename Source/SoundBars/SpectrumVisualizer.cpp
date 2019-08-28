// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrumVisualizer.h"
#include "TimeSynthComponent.h"
#include "Components/StaticMeshComponent.h"

ASpectrumVisualizer::ASpectrumVisualizer()
{
	PrimaryActorTick.bCanEverTick = true;

	SpectrumBarSpacing = 100.f;

	TimeSynthComponent = CreateDefaultSubobject<UTimeSynthComponent>("TimeSynthComponent");
	TimeSynthComponent->bEnableSpectralAnalysis = true;

	for (int32 i = 1; i <= 20; i++)
	{
		TimeSynthComponent->FrequenciesToAnalyze.Add(i * 50);

		FString SpectrumBarName = FString::Printf(TEXT("SpectrumBar%dHz"), i*50);

		UStaticMeshComponent* SpectrumBar = CreateDefaultSubobject<UStaticMeshComponent>(*SpectrumBarName);
		SpectrumBar->SetupAttachment(TimeSynthComponent);
		SpectrumBar->SetStaticMesh(SpectrumBarMesh);
		SpectrumBar->SetRelativeLocation(FVector((i * SpectrumBarSpacing), 0.f, 0.f));
		SpectrumBars.Add(SpectrumBar);
	}
}

void ASpectrumVisualizer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Refresh();
}

void ASpectrumVisualizer::Refresh()
{
	for (int32 i = 0; i <= 20; i++)
	{
		if (SpectrumBars.IsValidIndex(i))
		{
			UStaticMeshComponent* SpectrumBar = SpectrumBars[i];
			SpectrumBar->SetStaticMesh(SpectrumBarMesh);
			SpectrumBar->SetRelativeLocation(FVector((i+1) * SpectrumBarSpacing, 0.f, 0.f));
		}
	}
}

void ASpectrumVisualizer::BeginPlay()
{
	Super::BeginPlay();
	
	Refresh();

	TimeSynthComponent->PlayClip(TimeSynthClip);
}

void ASpectrumVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto SpecData : TimeSynthComponent->GetSpectralData())
	{
		UStaticMeshComponent* SpectrumBar = SpectrumBars[SpecData.FrequencyHz / 50.f - 1];

		FVector BarScale = SpectrumBar->GetComponentScale();
		BarScale.Z = 1.f + SpecData.Magnitude / 5.f;
		SpectrumBar->SetWorldScale3D(FMath::VInterpTo(SpectrumBar->GetComponentScale(), BarScale, DeltaTime, 6.f));
	}
}