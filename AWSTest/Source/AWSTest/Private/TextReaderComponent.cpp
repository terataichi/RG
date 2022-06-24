// Fill out your copyright notice in the Description page of Project Settings.


#include "TextReaderComponent.h"
#include "RunTime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"


UTextReaderComponent::UTextReaderComponent()
{

}


FString UTextReaderComponent::ReadFile(const FString& filePath)
{
	FString directoryPath = FPaths::ProjectContentDir();
	FString fullPath = directoryPath + "/" + filePath;

	FString result = {};
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();

	if (!file.FileExists(*fullPath))
	{
		check(!"ì«Ç›çûÇﬁFileÇ™ë∂ç›ÇµÇ»Ç¢");
	}
	FFileHelper::LoadFileToString(result, *fullPath);
	return result;
}

