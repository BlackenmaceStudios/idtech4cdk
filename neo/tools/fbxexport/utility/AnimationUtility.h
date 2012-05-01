/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#ifndef INCLUDE_ANIMATION_UTILITY_H_
#define INCLUDE_ANIMATION_UTILITY_H_



/** Create a default animation stack and a default animation layer for the given scene.
  * /param pScene The scene in which the animation stack and layer are created.
  * /param pAnimStack The created animation stack.
  * /return The created animation layer.
  */
KFbxAnimLayer * CreateDefaultAnimStackAndLayer(KFbxScene * pScene, KFbxAnimStack* &pAnimStack);

#endif // INCLUDE_ANIMATION_UTILITY_H_
