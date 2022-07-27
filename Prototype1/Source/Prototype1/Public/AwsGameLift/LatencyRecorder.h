// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UUnrealFpsGameInstance;

/**
 * 
 */
class PROTOTYPE1_API LatencyRecorder
{
public:
	static float GetAveragePlayerLatency(UUnrealFpsGameInstance* fpsGameInstance);

private:
	LatencyRecorder();
	static float averagePlayerLatency_;
	// コピー、代入禁止
	LatencyRecorder(const LatencyRecorder&) = delete;
	void operator=(const LatencyRecorder&) = delete;
};
