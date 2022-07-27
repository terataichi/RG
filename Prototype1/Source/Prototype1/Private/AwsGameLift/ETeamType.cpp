// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameLift/ETeamType.h"

namespace
{
	static const TMap<TEAMTYPE, FString> teamNameMap = {
		{TEAMTYPE::MIN,"Error"},
		{TEAMTYPE::RED,"Red"},
		{TEAMTYPE::BLUE,"Blue"},
		{TEAMTYPE::MAX,"Error"},
		{TEAMTYPE::TYPE_ERROR,"Error"}
	};
}

const TEAMTYPE ETeamType::GetRandomTeamType(void)
{
	return static_cast<TEAMTYPE>(FMath::RandRange(static_cast<int32>(TEAMTYPE::MIN) + 1, static_cast<int32>(TEAMTYPE::MAX) - 1));
}

const FString ETeamType::GetRandomTeamType2String(void)
{
	TEAMTYPE type = GetRandomTeamType();
	return TeamType2FString(type);
}

const FString ETeamType::TeamType2FString(const TEAMTYPE& teamType)
{
	if (teamType == TEAMTYPE::MIN || teamType == TEAMTYPE::MAX, teamType == TEAMTYPE::TYPE_ERROR)
	{
		check(!"teamTypeが不正な値");
		return FString();
	}

	const FString* reStr = teamNameMap.Find(teamType);
	if (reStr == nullptr)
	{
		check(!"Mapに値が存在しない");
		return FString();
	}

	return *reStr;
}

const TEAMTYPE ETeamType::FString2TeamType(const FString& teamString)
{
	const TEAMTYPE* type = teamNameMap.FindKey(teamString);

	if (type == nullptr)
	{
		check(!"Mapに値が存在しない");
		return TEAMTYPE::TYPE_ERROR;
	}

	return *type;
}
