

#include "DataAssetsThumbnailsModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "DataAssetsThumbnailsRenderer.h"

IMPLEMENT_GAME_MODULE(FDataAssetsThumbnailsModule, DataAssetsThumbnails);

UE_DISABLE_OPTIMIZATION
void FDataAssetsThumbnailsModule::StartupModule()
{
    // Register custom thumbnail renderers
    UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UDataAssetsThumbnailsRenderer::StaticClass());
}

void FDataAssetsThumbnailsModule::ShutdownModule()
{
    // Unregister thumbnail renderers?
    if (UThumbnailManager* ThumbnailManager = UThumbnailManager::TryGet())
    {
        if (IsValid(ThumbnailManager) && (ThumbnailManager->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed) == false))
        {
            ThumbnailManager->UnregisterCustomRenderer(UDataAsset::StaticClass());
        }
    }
}
UE_ENABLE_OPTIMIZATION