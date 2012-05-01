/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include "precompiled.h"

#include "../FbxExport_Local.h"


KFbxAnimLayer * CreateDefaultAnimStackAndLayer(KFbxScene * pScene, KFbxAnimStack* &pAnimStack)
{
    const char * DEFAULT_STACK_NAME = "Base_Stack";
    const char * DEFAULT_LAYER_NAME = "Base_Layer";

    // Create one animation stack
    pAnimStack = KFbxAnimStack::Create(pScene, DEFAULT_STACK_NAME);

    // all animation stacks need, at least, one layer.
    KFbxAnimLayer* lAnimLayer = KFbxAnimLayer::Create(pScene, DEFAULT_LAYER_NAME);
    pAnimStack->AddMember(lAnimLayer);

    return lAnimLayer;
}
