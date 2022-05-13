// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// ステージ１マスが持つステータス
UENUM()
enum class StageSpaceState
{
    None,      // 設置不可
    NotPut,    // 設置可
    Put,       // 設置中
};