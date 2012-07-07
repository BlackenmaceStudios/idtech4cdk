// Kinect_Stream.cpp
//

#include "precompiled.h"

#include "kinect_local.h"

/*
==================
bmKinectStream::OpenStream
==================
*/
void bmKinectStream::OpenStream( NUI_IMAGE_TYPE cameraType )
{
	HRESULT hr;
	NUI_IMAGE_RESOLUTION res;

	res = NUI_IMAGE_RESOLUTION_640x480;

	common->Printf("Opening Camera Stream %d\n", cameraType );
	m_hNextFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hr = kinectDeviceLocal.pNuiSensor->NuiImageStreamOpen(cameraType,res,0,2,m_hNextFrameEvent,&m_pStreamHandle);

	if(FAILED(hr)) {
		common->FatalError( "KinectStream::OpenStream: Failed");
	}

	pTexture = NULL;
}
/*
==================
bmKinectStream::CloseStream
==================
*/
void bmKinectStream::CloseStream( void ) {
	if(pTexture != NULL) {
		// We're done with the texture so unlock it
		pTexture->UnlockRect(0);

		// Release the frame
		kinectDeviceLocal.pNuiSensor->NuiImageStreamReleaseFrame(m_pStreamHandle, &imageFrame);
		pTexture = NULL;
	}
}

/*
==================
bmKinectStream::GetStreamBuffer
==================
*/
byte* bmKinectStream::GetStreamBuffer( int &size, int &pitch ) {
	HRESULT hr;

	kinectDeviceLocal.CloseAllStreams();

    // Attempt to get the color frame
    hr = kinectDeviceLocal.pNuiSensor->NuiImageStreamGetNextFrame(m_pStreamHandle, 0, &imageFrame);
    if (FAILED(hr))
    {
		size = 0;
		pitch = 0;
        return NULL;
    }

    pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    pTexture->LockRect(0, &LockedRect, NULL, 0);
	size = LockedRect.size;
	pitch = LockedRect.Pitch;

	return LockedRect.pBits;
}