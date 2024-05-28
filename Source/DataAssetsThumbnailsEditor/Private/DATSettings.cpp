// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "DATSettings.h"

UDATSettings* UDATSettings::Get()
{
	return GetMutableDefault<UDATSettings>();
}
