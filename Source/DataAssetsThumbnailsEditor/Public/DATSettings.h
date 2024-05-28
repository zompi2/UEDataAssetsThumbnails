// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DATSettings.generated.h"

/**
 * Storing settings in DefaultEditor.ini file.
 */

UCLASS(config = Editor, defaultconfig)
class DATAASSETSTHUMBNAILSEDITOR_API UDATSettings : public UObject
{
    GENERATED_BODY()

public:

    static UDATSettings* Get();

private:


};
