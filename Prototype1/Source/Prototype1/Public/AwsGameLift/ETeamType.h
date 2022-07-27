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
	/// �����_����TEAMTYPE��Ԃ�
	/// </summary>
	/// <returns>TEAMTYPE</returns>
	static const TEAMTYPE GetRandomTeamType(void);

	/// <summary>
	/// �����_����TEAMTYPE�ɑΉ�����HString��Ԃ�
	/// </summary>
	/// <returns>TEAMTYPE�ɑΉ�����FString</returns>
	static const FString GetRandomTeamType2String(void);

	/// <summary>
	/// TEAMTYPE��FString�ɕϊ�����
	/// </summary>
	/// <param name="teamType">�ϊ�������Type</param>
	/// <returns>�ϊ����ꂽType</returns>
	static const FString TeamType2FString(const TEAMTYPE& teamType);

	static const TEAMTYPE FString2TeamType(const FString& teamString);
private:
	ETeamType();

	// �R�s�[�A����֎~
	ETeamType(const ETeamType&) = delete;
	void operator=(const ETeamType&) = delete;
};
