// Fill out your copyright notice in the Description page of Project Settings.


#include "LatencyRecorder.h"
#include "UnrealFpsGameInstance.h"

float LatencyRecorder::averagePlayerLatency_ = 0;


namespace
{
	constexpr float DEFAULT_PLAYERLATENCY = 60.0f;
}

float LatencyRecorder::GetAveragePlayerLatency(UUnrealFpsGameInstance* fpsGameInstance)
{
	if (fpsGameInstance == nullptr)
	{
		check(!"fpsGameInstance‚ªnullptr");
		return DEFAULT_PLAYERLATENCY;
	}
	float totalPlayerLatency = 0.0f;
	const auto& playerLatencyList = fpsGameInstance->GetPlayerLatencies();
	if (playerLatencyList.Num() <= 0)
	{
		return DEFAULT_PLAYERLATENCY;
	}
	for (float playerLatency : playerLatencyList)
	{
		totalPlayerLatency += playerLatency;
	}

	if (totalPlayerLatency < 0.0f)
	{
		check(!"totalPlayerLatency‚ª0");
		return DEFAULT_PLAYERLATENCY;
	}

	averagePlayerLatency_ = totalPlayerLatency / playerLatencyList.Num();

	return averagePlayerLatency_;
}

LatencyRecorder::LatencyRecorder()
{
}

