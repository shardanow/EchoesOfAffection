// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayRuntime.h"

#define LOCTEXT_NAMESPACE "FTimeOfDayRuntimeModule"

void FTimeOfDayRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
}

void FTimeOfDayRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTimeOfDayRuntimeModule, TimeOfDayRuntime)

