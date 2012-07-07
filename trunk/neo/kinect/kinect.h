// kinect.h
//

//
// bmKinectPlayer
//
class bmKinectPlayer {
public:
	virtual bool			IsTracking( void ) = 0;

	virtual void			GetJointTransform( int jointId, idQuat &angles ) = 0;
};

//
// bmKinectDevice
//
class bmKinectDevice {
public:
										// Init the kinect Driver.
	virtual void						Init( void ) = 0;

										// Returns the current elevation angle.
	virtual int							GetElevationAngle( void ) = 0;

										// Set the device elevation angle.
	virtual void						SetElevationAngle( int angle ) = 0;

										// Get Data from the color camera.
	virtual byte						*GetColorCameraData(int &size, int &pitch ) = 0;

										// Get Data from the depth camera.
	virtual byte						*GetDepthCameraData(int &size, int &pitch ) = 0;

	virtual unsigned short					DepthPixelToDepth( byte depth ) = 0;

	virtual bmKinectPlayer				*GetTrackedPlayer( int playerId ) = 0;

	virtual void						NextFrame( void ) = 0;
};

extern bmKinectDevice					*kinectDevice;