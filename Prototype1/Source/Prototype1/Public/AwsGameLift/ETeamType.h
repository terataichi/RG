// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


enum class TEAMTYPE
{
	MIN,
	RED,
	BLUE,
	MAX,
	TYPE_ERROR
};

/**
 * 
 */
class PROTOTYPE1_API ETeamType
{
public:
	/// <summary>
	/// ランダムにTEAMTYPEを返す
	/// </summary>
	/// <returns>TEAMTYPE</returns>
	static const TEAMTYPE GetRandomTeamType(void);

	/// <summary>
	/// ランダムにTEAMTYPEに対応したHStringを返す
	/// </summary>
	/// <returns>TEAMTYPEに対応したFString</returns>
	static const FString GetRandomTeamType2String(void);

	/// <summary>
	/// TEAMTYPEをFStringに変換する
	/// </summary>
	/// <param name="teamType">変換したいType</param>
	/// <returns>変換されたType</returns>
	static const FString TeamType2FString(const TEAMTYPE& teamType);

	static const TEAMTYPE FString2TeamType(const FString& teamString);
private:
	ETeamType();

	// コピー、代入禁止
	ETeamType(const ETeamType&) = delete;
	void operator=(const ETeamType&) = delete;
};
