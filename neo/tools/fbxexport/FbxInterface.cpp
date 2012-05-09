// FbxInterface.cpp
//

#include "precompiled.h"

#include "FbxExport_Local.h"

FbxInterface	fbxInterface;

/*
===============
FbxInterface::FbxInterface
===============
*/
FbxInterface::FbxInterface() {
	lSdkManager = NULL;
	lScene = NULL;
}

/*
===============
FbxInterface::Init
===============
*/
void FbxInterface::Init( void ) {
	// Clear the buffers.
	meshes.Clear();


	importError = false;

	if(lSdkManager != NULL) {
		common->Printf( "FBX Interface already started. Skipping init...\n");
		return;
	}
	common->Printf( "Init FBX Interface...\n");

	InitializeSdkObjects(lSdkManager, lScene);
}

/*
===============
FbxInterface::LoadFBX
===============
*/
bool FbxInterface::LoadFBX( const char *fileName ) {
	common->Printf("LoadFbx: Loading %s\n", fileName );
	meshes.Clear();
	verts.Clear();
	indexes.Clear();
	if(!LoadScene( lSdkManager, lScene, fileName )) {
		return false;
	}

	KFbxNode* lNode = lScene->GetRootNode();
	for( int i = 0; i < lNode->GetChildCount(); i++) {
		ParseNode( lNode->GetChild(i) );
	}

	return true;
}

/*
===============
FbxInterface::ParseNode
===============
*/
void FbxInterface::ParseNode( KFbxNode* pNode ) {
	 KFbxNodeAttribute::EAttributeType lAttributeType;

	 if(pNode->GetNodeAttribute() == NULL)
     {
        common->Warning("EXPORT ERROR: NULL Node Attribute\n\n");
		return;
     }

	 lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());
	 switch (lAttributeType)
	 {
		 case KFbxNodeAttribute::eMESH:
			 ParseMesh( pNode );
			 break;
		 default:
			 common->Warning("Ignorning node %s - no code handler implemented.\n", pNode->GetName() );
			 break;
	 }
}

/*
===============
FbxInterface::ParseMeshTexture
===============
*/
void FbxInterface::ParseMeshTexture( KFbxGeometry* pGeometry, bmFbxMesh *Mesh ) {
	int lMaterialIndex;
    KFbxProperty pProperty;
    if(pGeometry->GetNode()==NULL) {
		ImportError( "EXPORT ERROR: %s has no assigned material\n", pGeometry->GetName() );
        return;
	}

	int lNbMat = pGeometry->GetNode()->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
	 for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++){
        KFbxSurfaceMaterial *lMaterial = (KFbxSurfaceMaterial *)pGeometry->GetNode()->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex);
        bool lDisplayHeader = true;

        //go through all the possible textures
        if(lMaterial){

            int lTextureIndex;
            FOR_EACH_TEXTURE(lTextureIndex)
            {
                pProperty = lMaterial->FindProperty(KFbxLayerElement::TEXTURE_CHANNEL_NAMES[lTextureIndex]);
				if( pProperty.IsValid() )
				{
					int lTextureCount = pProperty.GetSrcObjectCount(KFbxTexture::ClassId);

					if(strcmp( "DiffuseColor", pProperty.GetName().Buffer())) {
						continue;
					}

					for (int j = 0; j < lTextureCount; ++j)
					{
						//no layered texture simply get on the property
						KFbxTexture* pTexture = KFbxCast <KFbxTexture> (pProperty.GetSrcObject(KFbxTexture::ClassId,j));
						KFbxFileTexture *lFileTexture = KFbxCast<KFbxFileTexture>(pTexture);


						Mesh->material = fileSystem->OSPathToRelativePath( "worlddefault" );
						Mesh->material.StripFileExtension();

						common->Printf("\tMaterial %s\n",  Mesh->material.c_str());
					}
					
				}
                //FindAndDisplayTextureInfoByProperty(lProperty, lDisplayHeader, lMaterialIndex); 


            }

        }//end if(lMaterial)

    }// end for lMaterialIndex    
}

/*
===============
FbxInterface::ImportError
===============
*/
void FbxInterface::ImportError( const char *fmt, ... ) {
	char		msg[1024];
	importError = true;

	va_list argptr;
	va_start( argptr, fmt );
	idStr::vsnPrintf( msg, 1024E-1, fmt, argptr );
	common->Warning("Import Failure: %s\n", msg );
	va_end( argptr );
	
}

/*
===============
FbxInterface::ParseMeshGeometry
===============
*/
void FbxInterface::ParseMeshGeometry(KFbxMesh* pMesh, bmFbxMesh *Mesh) {
	int lPolygonCount;
	KFbxVector4* lControlPoints = pMesh->GetControlPoints();
	KFbxLayerElementArrayTemplate<KFbxVector4> *normals;

	pMesh->GetNormals( &normals );

	lPolygonCount = pMesh->GetPolygonCount();
	if(lPolygonCount <= 0) {
		ImportError( "Missing polygons detected\n");
		return;
	}

	Mesh->numVertexes = pMesh->GetControlPointsCount();


	//get lUVSetIndex-th uv set
    
	int uvid = 0;

	// Load in the vertexes
	
	for(int i = 0; i < Mesh->numVertexes; i++) {
		idDrawVert vert;

		vert.xyz = idVec3( (float)lControlPoints[i][0], (float)-lControlPoints[i][2], (float)lControlPoints[i][1] );
		if(vert.xyz.IsInfinite()) {
			common->Error( "Infinite Vertex detected, check your mesh and try reexporting again.\n");
		}

		
		verts.Append( vert );
	}
	common->Printf("\t%d Vertexes\n", Mesh->numVertexes);


	

	// Load in the uvs.
	LoadUVInformation( pMesh, Mesh );
	
	Mesh->numIndexes = indexes.Num();
	if(Mesh->numIndexes % 3) {
		ImportError( "Mesh isn't triangulated. In the mesh export options ensure you have checked triangulate.\n");
		return;
	}
	/*
	for(int i = 0; i < indexes.Num(); i+=3)
	{
			  int ind = i;
			  int t = indexes[ind + 1 ];
			  indexes[ind + 1 ] = indexes[ind + 2 ];
			  indexes[ind + 2 ] = t;
	}
	*/
	common->Printf("\t%d Indexes\n", indexes.Num());
}

void  FbxInterface::LoadUVInformation(KFbxMesh* pMesh,  bmFbxMesh *Mesh)
{
    //get all UV set names
    KStringList lUVSetNameList;
	

    pMesh->GetUVSetNames(lUVSetNameList);

    //iterating over all uv sets
    for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
    {
        //get lUVSetIndex-th uv set
        const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
        const KFbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		KFbxLayerElementUV* uvs = pMesh->GetLayer(lUVSetIndex)->GetUVs();
		common->Printf("Loading UV's for set %s\n", lUVSetName);

        if(!lUVElement)
            continue;

        // only support mapping mode eBY_POLYGON_VERTEX and eBY_CONTROL_POINT
        if( lUVElement->GetMappingMode() != KFbxGeometryElement::eBY_POLYGON_VERTEX &&
            lUVElement->GetMappingMode() != KFbxGeometryElement::eBY_CONTROL_POINT )
		{
            common->Error("Illegal UVElement mapping mode\n");
		}

        //direct array, where holds the actual uv data
        const int lDataCount = lUVElement->GetDirectArray().GetCount();

        //index array, where holds the index referenced to the uv data
        const bool lUseIndex = lUVElement->GetReferenceMode() != KFbxGeometryElement::eDIRECT;
        const int lIndexCount= (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

        //iterating through the data by polygon
        const int lPolyCount = pMesh->GetPolygonCount();
		int vertexId = 0;
		for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex, vertexId++ )
		{
			// build the max index array that we need to pass into MakePoly
            if(pMesh->GetPolygonSize(lPolyIndex) != 3) {
				common->Error("Mesh no triangulated");
			}

			for( int lVertIndex = 0; lVertIndex < 3; ++lVertIndex )
            {
				
				//get the index of the current vertex in control points array
				int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex,lVertIndex);
				switch(lUVElement->GetMappingMode())
				{
					case KFbxGeometryElement::eBY_POLYGON_VERTEX:
						{
							// Set the UV's for the polygon.
							int uvIndex = pMesh->GetTextureUVIndex(lPolyIndex, lVertIndex);
							KFbxVector2& uv = uvs->GetDirectArray()[uvIndex];
							verts[lPolyVertIndex].st = idVec2( (float)uv[0], (float)uv[1] );

							// Set the normal for the polygon.
							KFbxVector4 fbxNormal;	
							pMesh->GetPolygonVertexNormal(lPolyIndex,lVertIndex, fbxNormal);	
							verts[lPolyVertIndex].normal = idVec3( (float)fbxNormal[0], (float)fbxNormal[1], (float)fbxNormal[2] );

							
						}
						break;

					case KFbxGeometryElement::eBY_CONTROL_POINT:  // this just adds too many vertexs for it to ever to be useful :/.
						{
							// Set the UV's for the polygon
							int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;
							KFbxVector2& uv = uvs->GetDirectArray()[lUVIndex];
							verts[lPolyVertIndex].st = idVec2( (float)uv[0], (float)uv[1] );

							// Set the normal for the polygon.
							KFbxVector4 fbxNormal;	
							pMesh->GetPolygonVertexNormal(lPolyIndex,lVertIndex, fbxNormal);	
							verts[lPolyVertIndex].normal = idVec3( (float)fbxNormal[0], (float)fbxNormal[1], (float)fbxNormal[2] );
						}
						break;

					default:
						common->FatalError("FBX mesh has unknown geometry element type!\n");
						break;
				}

				indexes.Append( lPolyVertIndex );
			}
		}
	}
/*
        if( lUVElement->GetMappingMode() == KFbxGeometryElement::eBY_CONTROL_POINT )
        {
            for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex )
            {
                // build the max index array that we need to pass into MakePoly
                const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
                for( int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex )
                {
                    KFbxVector2 lUVValue;

                    //get the index of the current vertex in control points array
                    int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex,lVertIndex);

                    //the UV index depends on the reference mode
                    int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

                    lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

                    //User TODO:
                    //Print out the value of UV(lUVValue) or log it to a file
					 verts[lPolyVertIndex].st = idVec2( lUVValue[0], 1.0f - lUVValue[1] );
                }
            }
        }
        else if (lUVElement->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX)
        {
         
            
                
                for( int lVertIndex = 0; lVertIndex < 3; lVertIndex++ )
                {
					KFbxVector2 lUVValue;



						 // Get the normal value.
						 KFbxVector4 fbxNormal;	
						pMesh->GetPolygonVertexNormal(lPolyIndex,lVertIndex, fbxNormal);	
						fbxNormal.Normalize();


					    int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex,lVertIndex);
						int lUVIndex = pMesh->GetTextureUVIndex(lPolyIndex, lVertIndex, KFbxLayerElement::eDIFFUSE_TEXTURES);


						 indexes.Append( lPolyVertIndex );
                        
                        lUVValue = uvs->GetDirectArray().GetAt(lUVIndex);

                       
						verts[lPolyVertIndex].st = idVec2( (float)lUVValue[0], 1.0f - (float)lUVValue[1] );
						verts[lPolyVertIndex].normal = idVec3((float)fbxNormal[0], (float)fbxNormal[1], (float)fbxNormal[2]);
                
                }
            }
        }
    }
*/
}

/*
===============
FbxInterface::ParseMesh
===============
*/
void FbxInterface::ParseMesh( KFbxNode* pNode ) {
	KFbxMesh* fbxMesh = (KFbxMesh*) pNode->GetNodeAttribute ();
	bmFbxMesh *bmMesh = AllocMesh( fbxMesh->GetName() );

	common->Printf("Importing Mesh...\n", bmMesh->name.c_str() );

	// Parse mesh texture 
	ParseMeshTexture( fbxMesh, bmMesh );

	// Get the mesh verts, uvs, and indexes.
	ParseMeshGeometry( fbxMesh, bmMesh );
}