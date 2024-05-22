// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class DATAASSETSTHUMBNAILS_API FDataAssetsThumbnailsModule : public IModuleInterface
{

protected:

	void StartupModule() override;
	void ShutdownModule() override;

private:

	FDelegateHandle GetVariableCustomizationInstanceDelegateHandle;
};
