/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/
#include "precompiled.h"

#include "../FbxExport_Local.h"

KFbxNode * CreatePyramid(KFbxScene * pScene, const char * pName, double pBottomWidth, double pHeight)
{
    KFbxMesh * lPyramid = KFbxMesh::Create(pScene, pName);

    // Calculate the vertices of the pyramid
    const double lBottomWidthHalf = pBottomWidth / 2;
    const KFbxVector4 PyramidControlPointArray[] = 
    {
        KFbxVector4(0, pHeight, 0),
        KFbxVector4(lBottomWidthHalf, 0, lBottomWidthHalf),
        KFbxVector4(lBottomWidthHalf, 0, -lBottomWidthHalf),
        KFbxVector4(-lBottomWidthHalf, 0, -lBottomWidthHalf),
        KFbxVector4(-lBottomWidthHalf, 0, lBottomWidthHalf)
    };

    // Initialize and set the control points of the mesh
    const int lControlPointCount = sizeof(PyramidControlPointArray) / sizeof(KFbxVector4);
    lPyramid->InitControlPoints(lControlPointCount);
    for (int lIndex = 0; lIndex < lControlPointCount; ++lIndex)
    {
        lPyramid->SetControlPointAt(PyramidControlPointArray[lIndex], lIndex);
    }

    // Set the control point indices of the bottom side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(1);
    lPyramid->AddPolygon(4);
    lPyramid->AddPolygon(3);
    lPyramid->AddPolygon(2);
    lPyramid->EndPolygon();

    // Set the control point indices of the front side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(1);
    lPyramid->AddPolygon(2);
    lPyramid->EndPolygon();

    // Set the control point indices of the left side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(2);
    lPyramid->AddPolygon(3);
    lPyramid->EndPolygon();

    // Set the control point indices of the back side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(3);
    lPyramid->AddPolygon(4);
    lPyramid->EndPolygon();

    // Set the control point indices of the right side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(4);
    lPyramid->AddPolygon(1);
    lPyramid->EndPolygon();

    // Attach the mesh to a node
    KFbxNode * lPyramidNode = KFbxNode::Create(pScene, pName);
    lPyramidNode->SetNodeAttribute(lPyramid);

    // Set this node as a child of the root node
    pScene->GetRootNode()->AddChild(lPyramidNode);

    return lPyramidNode;
}


typedef double Vector4[4];
typedef double Vector2[2];

// Create a cube.
KFbxNode* CreateCube(KFbxScene* pScene, const char* pName, fbxDouble3& pLclTranslation)
{
    // indices of the vertices per each polygon
    static int vtxId[24] = {
        0,1,2,3, // front  face  (Z+)
        1,5,6,2, // right  side  (X+)
        5,4,7,6, // back   face  (Z-)
        4,0,3,7, // left   side  (X-)
        0,4,5,1, // bottom face  (Y-)
        3,2,6,7  // top    face  (Y+)
    };

    // control points
    static Vector4 lControlPoints[8] = {
        { -5.0,  0.0,  5.0, 1.0}, {  5.0,  0.0,  5.0, 1.0}, {  5.0,10.0,  5.0, 1.0},    { -5.0,10.0,  5.0, 1.0}, 
        { -5.0,  0.0, -5.0, 1.0}, {  5.0,  0.0, -5.0, 1.0}, {  5.0,10.0, -5.0, 1.0},    { -5.0,10.0, -5.0, 1.0} 
    };

    // normals
    static Vector4 lNormals[8] = {
        {-0.577350258827209,-0.577350258827209, 0.577350258827209, 1.0}, 
        { 0.577350258827209,-0.577350258827209, 0.577350258827209, 1.0}, 
        { 0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0},
        {-0.577350258827209, 0.577350258827209, 0.577350258827209, 1.0}, 
        {-0.577350258827209,-0.577350258827209,-0.577350258827209, 1.0}, 
        { 0.577350258827209,-0.577350258827209,-0.577350258827209, 1.0},
        { 0.577350258827209, 0.577350258827209,-0.577350258827209, 1.0},
        {-0.577350258827209, 0.577350258827209,-0.577350258827209, 1.0}
    };

    // uvs
    static Vector2 lUVs[14] = {
        { 0.0, 1.0}, 
        { 1.0, 0.0}, 
        { 0.0, 0.0},
        { 1.0, 1.0}
    };

    // indices of the uvs per each polygon
    static int uvsId[24] = {
        0,1,3,2,2,3,5,4,4,5,7,6,6,7,9,8,1,10,11,3,12,0,2,13
    };

    // create the main structure.
    KFbxMesh* lMesh = KFbxMesh::Create(pScene,"");

    // Create control points.
    lMesh->InitControlPoints(8);
    KFbxVector4* vertex = lMesh->GetControlPoints();
    memcpy((void*)vertex, (void*)lControlPoints, 8*sizeof(KFbxVector4));

    // create the materials.
    /* Each polygon face will be assigned a unique material.
    */
    KFbxLayerContainer* lLayerContainer = (KFbxLayerContainer*)lMesh;
    KFbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
    lMaterialElement->SetMappingMode(KFbxGeometryElement::eALL_SAME);
    lMaterialElement->SetReferenceMode(KFbxGeometryElement::eINDEX_TO_DIRECT);

    lMaterialElement->GetIndexArray().Add(0);

    // Create polygons later after KFbxGeometryElementMaterial is created. Assign material indices.
    int vId = 0;
    for (int f=0; f<6; f++)
    {
        lMesh->BeginPolygon();
        for (int v=0; v<4; v++)
            lMesh->AddPolygon(vtxId[vId++]);
        lMesh->EndPolygon();
    }

    // specify normals per control point.
    KFbxGeometryElementNormal* lNormalElement = lMesh->CreateElementNormal();
	lNormalElement->SetMappingMode(KFbxGeometryElement::eBY_CONTROL_POINT);
    lNormalElement->SetReferenceMode(KFbxGeometryElement::eDIRECT);

    for (int n=0; n<8; n++)
        lNormalElement->GetDirectArray().Add(KFbxVector4(lNormals[n][0], lNormals[n][1], lNormals[n][2]));


    // Create the node containing the mesh
    KFbxNode* lNode = KFbxNode::Create(pScene,pName);
    lNode->LclTranslation.Set(pLclTranslation);

    lNode->SetNodeAttribute(lMesh);
    lNode->SetShadingMode(KFbxNode::eTEXTURE_SHADING);   
    
    // create UVset
    KFbxGeometryElementUV* lUVElement1 = lMesh->CreateElementUV("UVSet1");
	K_ASSERT( lUVElement1 != NULL);
    lUVElement1->SetMappingMode(KFbxGeometryElement::eBY_POLYGON_VERTEX);
    lUVElement1->SetReferenceMode(KFbxGeometryElement::eINDEX_TO_DIRECT);
    for (int i = 0; i <4; i++)
        lUVElement1->GetDirectArray().Add(KFbxVector2(lUVs[i][0], lUVs[i][1]));

    for (int i = 0; i<24; i++)
        lUVElement1->GetIndexArray().Add(uvsId[i%4]);

    return lNode;
}

