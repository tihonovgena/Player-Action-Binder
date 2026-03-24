// Copyright 2024 Gennadii Tikhonov 2024. All Rights Reserved.


#include "PlayerActionSetData.h"

static uint32 GSetID = 0;

UPlayerActionSetData::UPlayerActionSetData()
{
	Priority = 0;
	SetID = GSetID++;
}
