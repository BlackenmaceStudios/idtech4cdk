// Kinect_Main.cpp
//

#include "precompiled.h"

#include "kinect_local.h"

bmKinectDeviceLocal				kinectDeviceLocal;
bmKinectDevice					*kinectDevice = &kinectDeviceLocal;


/*
======================
bmKinectDeviceLocal::Init
======================
*/
void bmKinectDeviceLocal::Init( void ) {
	int iSensorCount = 0;
	common->Printf("--------- KinectDevice_Init -----------\n");

	NuiGetSensorCount(&iSensorCount);

	common->Printf("%d kinect devices attached...\n", iSensorCount );

	if(FAILED(NuiCreateSensorByIndex(0, &pNuiSensor))) {
		common->FatalError("Failed to alloc kinect sensor");
	}

	// Init the Kinect NUI interface
	common->Printf("Init NUI Interface...\n");
	if(FAILED(pNuiSensor->NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_AUDIO ))) {
		common->FatalError("Failed to init the Kinect NUI interface...\n");
		return;
	}

	common->Printf("Setting default kinect device angle.\n");
	SetElevationAngle( 0 );

	// Open the color streams
	cameraStream.OpenStream( NUI_IMAGE_TYPE_COLOR );
	depthStream.OpenStream( NUI_IMAGE_TYPE_DEPTH );

	common->Printf("Creating Players...\n");
	for(int i = 0; i < 7; i++) {
		players[i] = new bmKinectPlayerLocal( i );
	}
}

/*
======================
bmKinectDeviceLocal::GetElevationAngle
======================
*/
int bmKinectDeviceLocal::GetElevationAngle( void ) {
	LONG angle;
	if(FAILED(pNuiSensor->NuiCameraElevationGetAngle( &angle ))) {
		common->FatalError("Failed to get the camera angle\n");
		return -1;
	}

	return angle;
}

/*
======================
bmKinectDeviceLocal::SetElevationAngle
======================
*/
void bmKinectDeviceLocal::SetElevationAngle( int angle ) {
	if(GetElevationAngle() == angle)
		return;

	if(FAILED(pNuiSensor->NuiCameraElevationSetAngle( angle ))) {
		common->FatalError("Failed to set camera angle\n");
	}
}



/*
======================
bmKinectDeviceLocal::GetDepthCameraData
======================
*/
byte *bmKinectDeviceLocal::GetDepthCameraData(int &size, int &pitch ) {
	return depthStream.GetStreamBuffer( size, pitch );
}

/*
======================
bmKinectDeviceLocal::GetColorCameraData
======================
*/
byte *bmKinectDeviceLocal::GetColorCameraData(int &size, int &pitch ) {
	return cameraStream.GetStreamBuffer( size, pitch );
}
/*
======================
bmKinectDeviceLocal::DepthPixelToDepth
======================
*/
unsigned short bmKinectDeviceLocal::DepthPixelToDepth( byte depth ) { 
	return NuiDepthPixelToDepth( depth ); 
}
/*
======================
bmKinectDeviceLocal::NextFrame
======================
*/
void bmKinectDeviceLocal::NextFrame( void ) {
	NUI_SKELETON_FRAME skeletonFrame  = {0};

	memset( &skeletonFrame, 0, sizeof(NUI_SKELETON_FRAME) );

	HRESULT hr = pNuiSensor->NuiSkeletonGetNextFrame(6000, &skeletonFrame);
    if ( FAILED(hr) )
    {
        return;
    }

	// smooth out the skeleton data
    pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

	for(int i = 0; i < NUI_SKELETON_COUNT; i++) {
		players[i]->UpdatePlayer( skeletonFrame.SkeletonData[i] );
	}
}