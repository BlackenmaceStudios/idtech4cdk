//RenderDevice.h
//

//
// bmRenderDevice
//
class bmRenderDevice {
public:
	virtual void			Init( void ) = 0;
	virtual bool			BindDeviceToWindow( void *windowHandle ) = 0; 
	virtual void			SwapBuffers( void *windowHandle ) = 0;
	virtual void			SetDeviceState( int state ) = 0;
	virtual void			SetDevice2DState( void ) = 0;
	virtual void			DeriveLightData(class idRenderLightLocal *) = 0;
	virtual void			ToggleSMPFrame( void ) = 0;
	virtual void			ForceDeviceSync( void ) = 0;
	virtual void			BeginDeviceSync( void ) = 0;

	virtual void			Set2DViewMatrix( void ) = 0;
	virtual void			DrawImage(float x, float y, float w, float h, float s1, float t1, float s2, float t2, class idImage *image) = 0;
	virtual void			RestoreViewMatrix( void ) = 0;
	virtual void			RenderSurfaceFromCache( const struct srfTriangles_s *tris ) = 0;
};

extern bmRenderDevice		*renderDevice;