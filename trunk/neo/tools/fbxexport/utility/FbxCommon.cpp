/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/
#include "precompiled.h"

#include "../FbxExport_Local.h"

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif

void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene)
{
    // The first thing to do is to create the FBX SDK manager which is the 
    // object allocator for almost all the classes in the SDK.
    pSdkManager = KFbxSdkManager::Create();

    if (!pSdkManager)
    {
        common->Warning("Unable to create the FBX SDK manager\n");
        return;
    }

	// create an IOSettings object
	KFbxIOSettings * ios = KFbxIOSettings::Create(pSdkManager, IOSROOT );
	pSdkManager->SetIOSettings(ios);

	// Load plugins from the executable directory
	KString lPath = KFbxGetApplicationDirectory();
#if defined(KARCH_ENV_WIN)
	KString lExtension = "dll";
#elif defined(KARCH_ENV_MACOSX)
	KString lExtension = "dylib";
#elif defined(KARCH_ENV_LINUX)
	KString lExtension = "so";
#endif
	pSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

    // Create the entity that will hold the scene.
    pScene = KFbxScene::Create(pSdkManager,"");
}

void DestroySdkObjects(KFbxSdkManager* pSdkManager)
{
    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
    if (pSdkManager) pSdkManager->Destroy();
    pSdkManager = NULL;
}

bool SaveScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    KFbxExporter* lExporter = KFbxExporter::Create(pSdkManager, "");

    if( pFileFormat < 0 || pFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        //Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
        {
            if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
            {
                KString lDesc =pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                char *lASCII = "ascii";
                if (lDesc.Find(lASCII)>=0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        }
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.

    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pSdkManager->GetIOSettings()) == false)
    {
        common->Warning("Call to KFbxExporter::Initialize() failed. Error returned: %s\n\n\n", lExporter->GetLastErrorString());
        return false;
    }

    KFbxSdkManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    common->Printf("FBX version number for this version of the FBX SDK is %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pScene); 

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        common->Warning("Call to KFbxImporter::Initialize() failed.\n");
        common->Warning("Error returned: %s\n\n", lImporter->GetLastErrorString());

        if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
            lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
        {
            common->Printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            common->Printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    common->Printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        common->Printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        common->Printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        common->Printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        common->Printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        common->Printf("\n");

        for(i = 0; i < lAnimStackCount; i++)
        {
            KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            common->Printf("    Animation Stack %d\n", i);
            common->Printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            common->Printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            common->Printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            common->Printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            common->Printf("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
    {
        common->Warning("Password protected FBX files not supported\n");
		return false;
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}
