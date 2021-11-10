#pragma once
#ifndef SCENE_TYPES
#define SCENE_TYPES
namespace SCENE_TYPE
{
  enum ID {MENU,
    MODEL_TYPE_SELECT,
    PLAY_MODEL_SELECT,
    DATASET_MENU,
    DATASET_BUILDER,
    DATASET_BUILDER_PIX2PIX,
    DATASET_BUILDER_PIX2PIX_MANUAL,
    DATASET_PROCESSESOR,
    DATASET_SELECTOR,
    ARCHITECTURE_MENU,
    ARCHITECTURE_BUILDER,
    ARCHITECTURE_SELECT,
    HYPERPARAMETER_SELECT,
    TRAIN,
    INTERACT_MENU,
    INTERACT_EXPORT_GIF,
    EXPLORE_LATENT,
    INTERACT_MOTION,
    INTERACT_SYNC_MUSIC,
    INTERACT_DRAW_P2P,
    INTERACT_WEBCAM_P2P,
    INTERACT,
    INTERACTGAN,
    INTERACTPIX2PIX
    };
}
#endif


#ifndef MODEL_TYPES
#define MODEL_TYPES
namespace MODEL_TYPE
{
  enum ID {GAN, PIX2PIX, CUSTOM
    };
}
#endif
