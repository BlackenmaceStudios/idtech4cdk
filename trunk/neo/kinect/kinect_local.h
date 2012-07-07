// kinect_local.h
//

#include <NuiApi.h>



//
// bmKinectPlayerLocal
//
class bmKinectPlayerLocal : public bmKinectPlayer {
public:
	bmKinectPlayerLocal( int idx ) { playerIndex = idx; };

	void UpdatePlayer( NUI_SKELETON_DATA &skeletonData );
	virtual bool			IsTracking( void );
	virtual void			GetJointTransform( int jointId, idQuat &angles );
	virtual void			GetSkeletonJointPosition( int jointId, idVec3 &pos );

	static idMat3 KinectMatrixToEngineMatrix( Matrix4 &m );
private:
	int	playerIndex;
	NUI_SKELETON_DATA skeletonData;
	NUI_SKELETON_BONE_ORIENTATION skeletonOrientation[20];

	idAngles	joints[20];
};

ID_INLINE idMat3 bmKinectPlayerLocal::KinectMatrixToEngineMatrix( Matrix4 &m ) {
	return idMat3( m.M11, m.M12, m.M13,
				   m.M21, m.M22, m.M23,
				   m.M31, m.M32, m.M33 );
}

class bmKinectStream {
public:
	void					OpenStream( NUI_IMAGE_TYPE type );
	byte					*GetStreamBuffer(  int &size, int &pitch );
	void					CloseStream( void );
private:
	HANDLE                  m_pStreamHandle;
    HANDLE                  m_hNextFrameEvent;

	NUI_IMAGE_FRAME			imageFrame;
	INuiFrameTexture		*pTexture;
};

//
// bmKinectDeviceLocal
//
class bmKinectDeviceLocal : public bmKinectDevice {
friend class bmKinectStream;
public:
	virtual void		Init( void );

						// Returns the current elevation angle.
	virtual int			GetElevationAngle( void );

						// Set the device elevation angle.
	virtual void		SetElevationAngle( int angle );

	virtual byte		*GetColorCameraData(int &size, int &pitch);
	virtual byte		*GetDepthCameraData(int &size, int &pitch);
	virtual unsigned short					DepthPixelToDepth( byte depth );
	virtual bmKinectPlayer				*GetTrackedPlayer( int playerId ) { return players[playerId]; }
	virtual void						NextFrame( void );
protected:
	INuiSensor			*pNuiSensor;
	bmKinectStream		cameraStream;
	bmKinectStream		depthStream;

	void				CloseAllStreams( void ) { cameraStream.CloseStream(); depthStream.CloseStream(); };

private:
	bmKinectPlayerLocal *players[ 7 ];
	
};

extern bmKinectDeviceLocal				kinectDeviceLocal;