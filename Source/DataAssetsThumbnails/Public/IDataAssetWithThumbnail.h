// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "UObject/Interface.h"
#include "IDataAssetWithThumbnail.generated.h"

UINTERFACE()
class DATAASSETSTHUMBNAILS_API UDataAssetWithThumbnail : public UInterface
{
	GENERATED_BODY()
};

class DATAASSETSTHUMBNAILS_API IDataAssetWithThumbnail
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent)
	UObject* GetAssetToRender() const;
};
