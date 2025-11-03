// Copyright Epic Games, Inc. All Rights Reserved.

#include "RelationshipSystemCore.h"

#define LOCTEXT_NAMESPACE "FRelationshipSystemCoreModule"

DEFINE_LOG_CATEGORY(LogRelationshipSystem);

void FRelationshipSystemCoreModule::StartupModule()
{
	UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSystemCore module started"));
}

void FRelationshipSystemCoreModule::ShutdownModule()
{
	UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSystemCore module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRelationshipSystemCoreModule, RelationshipSystemCore)
