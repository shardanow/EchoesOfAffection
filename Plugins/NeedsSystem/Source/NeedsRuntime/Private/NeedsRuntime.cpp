// Copyright Epic Games, Inc. All Rights Reserved.

#include "NeedsRuntime.h"

#define LOCTEXT_NAMESPACE "FNeedsRuntimeModule"

void FNeedsRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
}

void FNeedsRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNeedsRuntimeModule, NeedsRuntime)
